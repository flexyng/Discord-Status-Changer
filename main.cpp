#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h>


const std::string TOKEN = "tu_token_aqui";  


size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

int main() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error inicializando curl" << std::endl;
        return 1;
    }

    
    const std::string states[] = {
        "status 1",
        "status 2",
        "status 3",
        "status 4"
    };
    int numStates = sizeof(states) / sizeof(states[0]);
    int currentState = 0;

    std::cout << "Iniciando cambiador de custom status. Presiona Ctrl+C para salir." << std::endl;
    std::cout << "¡ADVERTENCIA: Esto viola TOS de Discord! Usa bajo riesgo." << std::endl;

    while (true) {
        
        std::string json = R"({"custom_status": {"text": ")" + states[currentState] + R"("}})";
        
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/v9/users/@me/settings");
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, ("Authorization: " + TOKEN).c_str()));
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        
        res = curl_easy_perform(curl);
        readBuffer.clear();  

        if (res != CURLE_OK) {
            std::cerr << "Error en la petición: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Estado actualizado: " << states[currentState] << std::endl;
        }

        
        struct curl_slist* headers = nullptr;
        curl_easy_getinfo(curl, CURLINFO_HTTPHEADER, &headers);  
        curl_slist_free_all(headers);

        
        currentState = (currentState + 1) % numStates;

        
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}