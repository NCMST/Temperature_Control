#include "WebServerManager.hpp"

// Constructorul clasei
WebServerManager::WebServerManager(const char *ssid, const char *password)
    : ssid(ssid), password(password), server(HTTP_PORT)
{
    // Pagina HTML de start
    homePage = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Home Page</title>
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
        <style>
            body {
                font-family: Arial, sans-serif;
            }
            #menu {
                height: 100%;
                width: 250px; /* Lățimea meniului */
                position: fixed;
                z-index: 1;
                top: 0;
                left: -250px; /* Ascuns inițial */
                background-color: #343a40; /* Culoarea de fundal a meniului */
                overflow-x: hidden;
                transition: 0.5s; /* Animare pentru deschidere/închidere */
                padding-top: 20px;
            }
            #menu a {
                padding: 10px 15px;
                text-decoration: none;
                font-size: 20px;
                color: white;
                display: block;
                transition: 0.3s;
            }
            #menu a:hover {
                color: #f1f1f1; /* Culoare la hover */
            }
            #burger {
                font-size: 30px;
                cursor: pointer;
                position: absolute; /* Poziție fixă pentru buton */
                top: 20px; /* Margine de sus */
                left: 20px; /* Margine de stânga */
                z-index: 2; /* Asigură-te că este deasupra altor elemente */
            }

            #main {
                transition: margin-left .5s; /* Animare pentru conținut */
                padding: 16px;
                margin-left: 250px; /* Lățimea meniului pentru a face loc */
            }
        </style>
    </head>
    <body>
        <div id="menu">
            <a href="/">Home</a>
            <a href="/graph">Temperature Graph</a>
            <a href="/list">Temperature List</a> <!-- Buton pentru lista temperaturilor -->
        </div>

        <div id="main">
            <span id="burger" onclick="toggleMenu()">&#9776;</span> <!-- Buton burger -->
            <h1>Welcome!</h1>
            <button class="btn btn-primary" onclick="location.href='/graph'">Open Graph</button>
            <button class="btn btn-secondary" onclick="location.href='/list'">View Temperature List</button> <!-- Buton pentru lista temperaturilor -->
        </div>

        <script>
            function toggleMenu() {
                const menu = document.getElementById("menu");
                if (menu.style.left === "0px") {
                    menu.style.left = "-250px"; // Ascunde meniul
                } else {
                    menu.style.left = "0"; // Afișează meniul
                }
            }
        </script>
    </body>
    </html>
    )rawliteral";

    // Pagina HTML pentru grafic
    graphPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Temperature Graph</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: Arial, sans-serif;
        }
        #menu {
            height: 100%;
            width: 250px; /* Lățimea meniului */
            position: fixed;
            z-index: 1;
            top: 0;
            left: -250px; /* Ascuns inițial */
            background-color: #343a40; /* Culoarea de fundal a meniului */
            overflow-x: hidden;
            transition: 0.5s; /* Animare pentru deschidere/închidere */
            padding-top: 20px;
        }
        #menu a {
            padding: 10px 15px;
            text-decoration: none;
            font-size: 20px;
            color: white;
            display: block;
            transition: 0.3s;
        }
        #menu a:hover {
            color: #f1f1f1; /* Culoare la hover */
        }
            #burger {
                font-size: 30px;
                cursor: pointer;
                position: absolute; /* Poziție fixă pentru buton */
                top: 20px; /* Margine de sus */
                left: 20px; /* Margine de stânga */
                z-index: 2; /* Asigură-te că este deasupra altor elemente */
            }

            #main {
                transition: margin-left .5s; /* Animare pentru conținut */
                padding: 16px;
                margin-left: 250px; /* Lățimea meniului pentru a face loc */
            }
    </style>
</head>
<body>
    <div id="menu">
        <a href="/">Home</a>
        <a href="/graph">Temperature Graph</a>
        <a href="/list">Temperature List</a> <!-- Buton pentru lista temperaturilor -->
    </div>

    <div id="main">
        <span id="burger" onclick="toggleMenu()">&#9776;</span> <!-- Buton burger -->
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
</body>
</html>
)rawliteral";

    // Pagina HTML pentru lista temperaturilor
    listPage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Temperature List</title>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
</head>
<body>
   <div id="menu">
       <a href="/">Home</a>
       <a href="/graph">Temperature Graph</a>
       <a href="/list">Temperature List</a> 
   </div>

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