#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string>
#include "engine.h"

#define NONE_PS            -1
#define SINE_WAVE_PS        0
#define SQUARE_WAVE_PS      1
#define SAWTOOTH_WAVE_PS    2

#define NUM_KEYS 3

#include "single_include/nlohmann/json.hpp"
using JSON = nlohmann::json;

#define BUFF_QTSOCK_OFFSET 4 /*offset for qt reading in*/
#define MAX_BUF_LEN 1024

enum VOICE { VOICE_0 = 0, VOICE_1 = 1, VOICE_2 = 2, VOICE_3 = 3 };


class QtUI {
protected:
    Engine *synth;
    int sockfd;
    int first_time_running;
    std::vector<std::string> m_presets;
    
    // board state
    int m_voice_selected;

    int m_prev_attack_val; 
    int m_prev_decay_val; 
    int m_prev_release_val; 
    int m_prev_sustain_val; 

public:
    QtUI(Engine *synth) 
        : synth(synth), sockfd(-1), first_time_running(1), m_voice_selected(0)
    {
        m_presets.push_back("Sine Wave");
        m_presets.push_back("Square Wave");
        m_presets.push_back("Sawtooth Wave");

        m_prev_attack_val = 0;
        m_prev_decay_val = 0;
        m_prev_release_val = 0;
        m_prev_sustain_val = 0;
    }
    
    QtUI(const QtUI& ui) 
        : synth(ui.synth), sockfd(ui.sockfd), 
          first_time_running(ui.first_time_running),
          m_voice_selected(ui.m_voice_selected)
    {
        for (auto preset : ui.m_presets) {
            m_presets.push_back(preset);
        }

        m_prev_attack_val = ui.m_prev_attack_val;
        m_prev_decay_val = ui.m_prev_decay_val;
        m_prev_release_val = ui.m_prev_release_val;
        m_prev_sustain_val = ui.m_prev_sustain_val;
    }
    
    ~QtUI() { if (sockfd != -1) close(sockfd); }

    void run();

private:
    void connect_with_qt(int connfd);
    void update_state(const JSON &board);
};

// update the engine based on the state of the board (given as JSON packet)
void QtUI::update_state(const JSON &board) {
    for (int i = 0; i < NUM_VOICES; i++) {
        std::string voice = "voice" + std::to_string(i);

        float gain = board["voices"][voice]["knob"]["level"];
        synth->update_voice_gain(i, gain / 100.f);

        if (board["voices"][voice]["button"]["mute"] > 0) {
            synth->toggle_voice_mute(i);
        }

        if (board["voices"][voice]["button"]["level"] > 0) {
            m_voice_selected = i;
        }
    }

    float global_gain = board["vol"]["knob"]["vol"];
    synth->update_global_gain(global_gain / 100.f);

    if (board["vol"]["button"]["vol"] > 0) {
        synth->toggle_global_mute();
    }

    int attack_val  = board["adsr"]["knob"]["attack"];
    int decay_val   = board["adsr"]["knob"]["decay"];
    int release_val = board["adsr"]["knob"]["release"];
    int sustain_val = board["adsr"]["knob"]["sustain"];

    if (m_prev_attack_val != attack_val) {
        synth->setAttackRate(attack_val / 20.f);
    }

    if (m_prev_decay_val != decay_val) {
        synth->setDecayRate(decay_val / 20.f);
    }

    if (m_prev_release_val != release_val) {
        synth->setReleaseRate(release_val / 20.f);
    }

    if (m_prev_sustain_val != sustain_val) {
        synth->setSustainLevel(sustain_val / 100.f);
    }

    m_prev_attack_val  = attack_val;
    m_prev_decay_val   = decay_val;
    m_prev_release_val = release_val;
    m_prev_sustain_val = sustain_val;
    

    int selected_preset = board["selected_preset"];

    switch (selected_preset) {
    case SINE_WAVE_PS:
        synth->load_sinewave(m_voice_selected, 440);
        break;
    case SQUARE_WAVE_PS:
        synth->load_square_wave(m_voice_selected, 200);
        break;
    case SAWTOOTH_WAVE_PS:
        synth->load_sawtooth(m_voice_selected, 200);
        break;
    default:
        break;
    }

    for (int i = 0; i < NUM_KEYS; i++) {
        std::string key = "key" + std::to_string(i);
        int key_pressed = board["key_pressed"][key];
        int key_released = board["key_released"][key];

        if (key_pressed > 0) {
            synth->gate_on();

        }
        
        if (key_released > 0) {
            synth->gate_off();
        }
    }
}


// Function designed for chat between Engine and Qt
void QtUI::connect_with_qt(int connfd) 
{ 
    char buff[MAX_BUF_LEN];

    while (1) { 
		// send message to Qt
        if (first_time_running) {
            JSON init_packet;

            for (std::string preset : m_presets) {
                init_packet["presets"].push_back(preset);
            }

            std::string msg = "initial_msg_" + init_packet.dump();
            write(connfd, msg.c_str(), msg.length());
            
            first_time_running = 0;
        }
		
		bzero(buff, MAX_BUF_LEN); 
		int num_bytes_read = read(connfd, buff, sizeof(buff));
		
		std::string msg_in;
		msg_in.append(buff+BUFF_QTSOCK_OFFSET, num_bytes_read-BUFF_QTSOCK_OFFSET);
		int msg_len = std::stoi(msg_in.substr(0, msg_in.find("_")));
		
		int bytes_remaining = msg_len - (msg_in.length() - msg_in.find("_") - 1);
		
		std::string msg = msg_in.substr(msg_in.find("_")+1);
		
		// read remaing buffer
		while (bytes_remaining >= 0) {
			bzero(buff, MAX_BUF_LEN);
			num_bytes_read = read(connfd, buff, sizeof(buff));
			bytes_remaining -= num_bytes_read;
			msg.append(buff, num_bytes_read);
		}

		JSON board_info = JSON::parse(msg);
		
        // interpret message //
        update_state(board_info);
    } 
} 

void QtUI::run() {
    std::cout << "running UI." << std::endl;
    //---set up code for socket server---//
	char *socket_path = "/tmp/socket_for_synth_eng";

	struct sockaddr_un addr;
	int connfd;

	// get file descriptor of socket
	if ( (sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
	unlink(socket_path);

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(-1);
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen error");
		exit(-1);
	}

    // while loop to connect with clients
	while (1) {
		if ( (connfd = accept(sockfd, NULL, NULL)) == -1) {
			perror("accept error");
			continue;
		}

		connect_with_qt(connfd); 
	}
	
	// After chatting close the socket 
	close(sockfd);
}
