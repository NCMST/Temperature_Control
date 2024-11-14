#include "WebServerManager.hpp"

// Constructorul clasei
WebServerManager::WebServerManager(const char *ssid, const char *password)
    : ssid(ssid), password(password), server(HTTP_PORT)
{

    String headContent = R"rawliteral(
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <script src="https://code.jquery.com/jquery-3.5.1.slim.min.js"></script> <!-- Include jQuery -->
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.5.2/dist/js/bootstrap.bundle.min.js"></script> <!-- Include Bootstrap JS -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script> <!-- Include Chart.js -->
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0; /* Elimină marginile implicite */
            padding-bottom: 60px; /* Asigură un spațiu pentru footer */
        }
        #main {
            padding-top: 60px; /* Asigură un spațiu pentru navbar */
            margin: 5%; /* Asigură un spațiu pentru navbar */
        }
        footer {
            background-color: #343a40; /* Culoarea de fundal a footer-ului */
            color: white; /* Culoarea textului */
            text-align: center; /* Centrarea textului */
            padding: 10px 0; /* Padding pentru footer */
            position: fixed; /* Fixează footer-ul la baza paginii */
            bottom: 0; /* Alinierea la baza paginii */
            width: 100%; /* Lățimea completă a footer-ului */
        }
    </style>
</head>
)rawliteral";

    String footerContent = R"rawliteral(
    <footer>
        <p>Project GitHub:<a style='color:white' href='https://github.com/username/repo'>GitHub Repository</a></p>
        <p>Author:<br/>Your Name</p>
        <p>Email for contact:<br/>your.email@example.com</p>
    </footer>
    )rawliteral";

    String navContent = R"rawliteral(
<div id="navbar" class="navbar navbar-expand-lg navbar-dark bg-dark">
    <a class="navbar-brand" href="/">Home</a>
    <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav" aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
        <span class="navbar-toggler-icon"></span>
    </button>
    <div class="collapse navbar-collapse" id="navbarNav">
        <div class="navbar-nav">
            <a class="nav-item nav-link" href="/graph">Temperature Graph</a>
            <a class="nav-item nav-link" href="/list">Temperature List</a>
        </div>
    </div>
</div>
)rawliteral";

    // Pagina HTML de start
    homePage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    )rawliteral" +
               headContent + R"rawliteral(
    <body>
        )rawliteral" +
               navContent + R"rawliteral(

        <div id="main">
            <h1>Setup for seting temperature!</h1>
            <button class="btn btn-primary" onclick="location.href='/graph'">Open Graph</button>
            <button class="btn btn-secondary" onclick="location.href='/list'">View Temperature List</button> <!-- Buton pentru lista temperaturilor -->
            
            <!-- Label-uri pentru temperaturi -->
            <div class="mt-4">

                <h3>Current Temperatures:</h3>

                <table class="table">
                    <thead>
                        <tr>
                        <th scope="col">#</th>
                        <th scope="col">Name</th>
                        <th scope="col">Value</th>
                        <th scope="col">Masurment unit</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                        <th scope="row">1</th>
                        <td>Inside Temperature</td>
                        <td><p id="insideTemperature"> -- </p></td>
                        <td>°C</td>
                        </tr>
                        <tr>
                        <th scope="row">2</th>
                        <td>Outside Temperature</td>
                        <td><p id="outsideTemperature"> -- </p></td>
                        <td>°C</td>
                        </tr>
                        <tr>
                        <th scope="row">3</th>
                        <td>Setpoint Temperature</td>
                        <td ><input type="text" class="form-control" aria-label="Temperature value (with dot and two decimal places)"></td>
                        <td>°C</td>
                        </tr>
                    </tbody>
                    </table>             
            </div>
        
        <!-- Scripturi -->
        <script>
            function updateTemperatures(insideTemp, outsideTemp) {
                document.getElementById("insideTemperature").innerText =  insideTemp;
                document.getElementById("outsideTemperature").innerText =  outsideTemp;
            }

            function fetchCurrentTemperatures() {
                fetch('/temperature') // Asigură-te că ai o rută corespunzătoare în server
                    .then(response => response.json())
                    .then(data => {
                        updateTemperatures(data.inside_temperature, data.outside_temperature);
                    })
                    .catch(error => console.error('Error fetching current temperatures:', error));
            }

            // Apelează funcția la încărcarea paginii și la intervale regulate
            fetchCurrentTemperatures();
            setInterval(fetchCurrentTemperatures, 5000); // Actualizează la fiecare 5 secunde
        </script>
        </div>
        )rawliteral" +
               footerContent + R"rawliteral(
        </body>
        </html>
        )rawliteral";

    // Pagina HTML pentru grafic
    graphPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
)rawliteral" +
               headContent + R"rawliteral(
<body>
    )rawliteral" +
               navContent + R"rawliteral(
     <div id="main">
                    <h1>Temperature Graph</h1>    
                    <canvas id="temperatureChart" width="400" height="200"></canvas>

                    <script>
                            const ctx = document.getElementById('temperatureChart').getContext('2d');
                            const temperatureChart = new Chart(ctx, {
                                type: 'line',
                                data: {
                                    labels: [], // Etichete pentru axa X
                                    datasets: [{
                                        label: 'Temperature (°C)',
                                        data: [], // Datele temperaturii
                                        borderColor: 'rgba(75, 192, 192, 1)',
                                        borderWidth: 1
                                    }]
                                },
                                options: {
                                    scales: {
                                        y: {
                                            beginAtZero: true
                                        }
                                    }
                                }
                            });

                            function updateTemperatureData(temperature) {
                                const currentTime = new Date().toLocaleTimeString(); // Obține timpul curent
                                temperatureChart.data.labels.push(currentTime);
                                temperatureChart.data.datasets[0].data.push(temperature);
                                temperatureChart.update();
                            }

                            function fetchTemperature() {
                                fetch('/temperature')
                                    .then(response => response.json())
                                    .then(data => {
                                        updateTemperatureData(data.inside_temperature); // Actualizează graficul cu temperatura primită
                                    })
                                    .catch(error => console.error('Error fetching temperature data:', error));
                            }

                            function toggleMenu() {
                                const menu = document.getElementById("menu");
                                if (menu.style.left === "0px") {
                                    menu.style.left = "-250px"; // Ascunde meniul
                                } else {
                                    menu.style.left = "0"; // Afișează meniul
                                }
                            }

                            setInterval(fetchTemperature, 5000); // Actualizează la fiecare 5 secunde
                        </script>
                        </div>
   
    )rawliteral" +
               footerContent + R"rawliteral(

</body>
</html>
)rawliteral";

    // Pagina HTML pentru lista temperaturilor
    listPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
)rawliteral" +
               headContent + R"rawliteral(
<body>
)rawliteral" + navContent + R"rawliteral(
   <div id="main">
       <h2>Temperature List</h2>
       <table class="table table-striped">
           <thead>
               <tr>
                   <th scope="col">Timestamp</th>
                   <th scope="col">Inside Temperature (°C)</th>
                   <th scope="col">Outside Temperature (°C)</th>
               </tr>
           </thead>
           <tbody id="temperatureTableBody">
               <!-- Datele vor fi populate aici -->
           </tbody>
       </table>
   </div>

   <script>
       function fetchTemperatureList() {
           fetch('/temperature/list') // Asigură-te că ai o rută corespunzătoare în server
               .then(response => response.json())
               .then(data => {
                   const tableBody = document.getElementById('temperatureTableBody');
                   tableBody.innerHTML = ''; // Resetează tabelul
                   data.forEach(entry => {
                       const row = `<tr><td>${entry.timestamp}</td><td>${entry.inside_temperature}</td><td>${entry.outside_temperature}</td></tr>`;
                       tableBody.innerHTML += row;
                   });
               })
               .catch(error => console.error('Error fetching temperature list:', error));
       }

       // Apelează funcția la încărcarea paginii
       fetchTemperatureList();
   </script>
   )rawliteral" +
               footerContent + R"rawliteral(

</body>
</html>
)rawliteral";
}

// Funcția de inițializare a serverului și conectare la Wi-Fi
void WebServerManager::begin()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password); // Conectează-te la Wi-Fi

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(1000);
        Serial.println("Conectare la WiFi...");
    }

    Serial.println("Conectat la WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", [this]()
              { handleHome(); }); // Setează ruta principală
    server.on("/graph", [this]()
              { handleGraph(); }); // Setează ruta pentru grafic
    server.on("/temperature", [this]()
              { handleTemperatureData(); }); // Setează ruta pentru datele temperaturii
    server.on("/list", [this]()
              { handleTemperatureList(); }); // Setează ruta pentru datele temperaturii in lista

    server.begin(); // Pornește serverul
}

// Funcția de gestionare a cererilor pentru pagina principală
void WebServerManager::handleHome()
{
    server.send(200, "text/html", homePage);
}

// Funcția de gestionare a cererilor pentru pagina graficului
void WebServerManager::handleGraph()
{
    server.send(200, "text/html", graphPage);
}

// Funcția de gestionare a cererilor pentru datele temperaturii
void WebServerManager::handleTemperatureData()
{

    DynamicJsonDocument doc(1024);
    doc["inside_temperature"] = currentTemperature.inside_temperature;
    doc["outside_temperature"] = currentTemperature.outside_temperature;

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server.send(200, "application/json", jsonResponse);
}

void WebServerManager::handleTemperatureList()
{
    DynamicJsonDocument doc(1024);

    JsonArray array = doc.to<JsonArray>();

    for (const auto &entry : temperatureHistory)
    {
        JsonObject obj = array.createNestedObject();
        obj["timestamp"] = entry.timestamp;
        obj["inside_temperature"] = entry.inside_temperature;
        obj["outside_temperature"] = entry.outside_temperature;
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    server.send(200, "application/json", jsonResponse);
}

// Funcția care primește temperatura dintr-un task
void WebServerManager::setTemperatureData(const TemperatureData &tempData)
{
    currentTemperature = tempData; // Stochează datele primite în variabila curentă.
}

// Funcția de gestionare a cererilor clientului
void WebServerManager::handleClient()
{
    server.handleClient(); // Gestionează cererile clientului.
}