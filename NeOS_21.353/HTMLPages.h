//===============================================////////// HTML PAGES //////////=======================================================================

// For inputting WiFi network details
const char HTML_WiFiConfig[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en-us">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Neo | WiFi Config</title>
    <link rel="icon" href="data:,">

    <style type="text/css">
        /* Import Open Sans Font From Google Servers */
        /*@import url('https://fonts.googleapis.com/css2?family=Open+Sans:wght@400;700&display=swap'); */
        /* Font Styling */
        h2 {
            display: block;
            color: black;
            font-family: 'Open Sans', sans-serif;
            font-size: clamp(23px, 10vw, 41.6px);
            line-height: 10px;
            margin-top: 0.75em;
            margin-bottom: 1em;
            margin-left: 0;
            margin-right: 0;
            font-weight: bold;
        }

        p,
        label {
            display: block;
            color: black;
            font-family: 'Open Sans', sans-serif;
            font-size: clamp(15px, 4.5vw, 19px);
            line-height: 10px;
            margin-top: 1em;
            margin-bottom: 1em;
            margin-left: 0;
            margin-right: 0;
        }

        /* Change Text Selection Color */
        ::selection,
        ::-moz-selection {
            color: white;
            background: #009933;
        }

        /* Wrapper Styling */
        html {
            width: 100vw;
            text-align: center;
        }

        body {
            width: 100vw;
            margin: 0 auto;
        }

        div {
            overflow: hidden;
        }

        form {}

        /* Object Styling */
        input {
            box-sizing: border-box;
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 28.5px;
            max-width: 370px;
            max-height: 30px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            font-size: clamp(14px, 3.5vw, 14.4px);
        }

        input[type=submit] {
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 30px;
            max-width: 370px;
            max-height: 40px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px #006600;
            border-radius: 5px;
            outline: none;
            background-color: #009933;
            color: white
        }

        input:focus[type=submit] {
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px #006600;
            border-radius: 5px;
            outline: none;
            box-shadow: 0 0 5px #006600;
            background-color: #00A435;
            color: white
        }

        select,
        option {
            box-sizing: border-box;
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 28.5px;
            max-width: 370px;
            max-height: 30px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            font-size: clamp(14px, 3.5vw, 14.4px);
        }

        input:focus,
        select:focus,
        option:hover {
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            outline-color: #006600;
            box-shadow: 0 0 5px #009933;
        }
    </style>
</head>

<body>
    <form >
        <h2>NEO<span style="color: #009933"> WIFI CONFIG</span></h2>
        <label for="SSID">WiFi SSID:</label>
        <p><input oninput="activateButton()" class="input" id="SSID" name="SSIDBox" type="text" placeholder="Ex: NeoNetwork" required="required"
                size="28" maxlength="30" /></p>
        <label for="PSK">WiFi Password:</label>
        <p><input class="input" id="PSK" name="PSKBox" type="password" placeholder="Ex: Password123" required="required"
                size="28" maxlength="30" /></p>
        <p><div id="connection">
          <input id="connect" style="background-color: #808080;" onclick="connectWiFi()" name="connectBtn" type="submit" value="CONNECT" />
        </div></p>
    </form>
    <p><div id="redirect">
       <input id="nextPage" onclick="Redirect()" name="redirectBtn" type="submit" value="NEXT" />
    </div></p>

    <script>
        var receivedString = "";
        var connectedFlag = "";
        var redirectLocation = "";
        var previouslyConnected = "Ex: NeoNetwork";
        var ranFlag = 0;
        var connectedOnce = 0;

        setInterval(function () {
            updateConnected();
        }, 2000);

        function activateButton(){
            if(document.getElementById("SSID").value != ''){
                document.getElementById("connect").style.backgroundColor = "#009933";
                document.getElementById("connect").value = "CONNECT";
            }
            else{
                document.getElementById("connect").style.backgroundColor = "#808080";
            }
        }
        function nextButton(){
            if(ranFlag == 0){
                document.getElementById("nextPage").value = "DEVICE OFFLINE";
                document.getElementById("nextPage").disabled = true;
                document.getElementById("nextPage").style.backgroundColor = "#808080";
            }
        }
        function updateConnected() {
            var xhttp = new XMLHttpRequest();
            nextButton();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    receivedString =  this.responseText;
                    let arr = [0,0,0];
                    arr[0] = receivedString.indexOf(',');
                    for(var i =1; i< 3; i++){
                        arr[i] = receivedString.indexOf(',',arr[i-1]+1);
                    }
                    connectedFlag = receivedString[0];
                    redirectLocation = receivedString.substring(arr[0]+1,arr[1]);
                    previouslyConnected = receivedString.substring(arr[1]+1,arr[2]);
                    console.log(redirectLocation);
                    console.log(previouslyConnected);
                    if(connectedFlag[0] == '1'){
                        if(connectedOnce == 0){
                            connectedOnce = 1;
                            document.getElementById("SSID").value = "";
                            document.getElementById("PSK").value = "";
                        }
                        document.getElementById("nextPage").value = "NEXT";
                        document.getElementById("SSID").placeholder = "Connected to: " + previouslyConnected;
                        document.getElementById("PSK").placeholder = "****************";
                        document.getElementById("nextPage").disabled = false;
                        document.getElementById("connect").value = "CONNECTED";
                        document.getElementById("nextPage").style.backgroundColor = "#009933";
                    }
                    else{
                        document.getElementById("nextPage").value = "DEVICE ONLINE";
                        document.getElementById("SSID").placeholder = "Ex: NeoNetwork";
                        document.getElementById("PSK").placeholder = "Ex: Password123";
                        document.getElementById("nextPage").disabled = true;
                        document.getElementById("nextPage").style.backgroundColor = "#808080";
                    }
                    ranFlag = 1;
                }
            };
            xhttp.open("GET", "isConnected", true);
            xhttp.send();
        }
        function connectWiFi() {
            if(document.getElementById("SSID").value != ''){
                let str = "<input id=\"connect\" style=\"background-color: #808080\" onclick=\"connectWiFi()\" name=\"connectBtn\" type=\"submit\" value=\"CONNECTING...\"/>";
                document.getElementById("connection").innerHTML = str;
                var xhr = new XMLHttpRequest();
                xhr.open("POST", "WiFiConfig", true);
                xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
                xhr.onload = function () {
                // do something to response
                console.log(this.responseText);
                };
                var ssid = document.getElementById("SSID").value;
                var password = document.getElementById("PSK").value;
                str = "SSIDBox=" + ssid + "&PSKBox=" +password;
                console.log(str);
                xhr.send(str);
            }
        }
        function Redirect() {
               window.location=redirectLocation;
            }
        nextButton();
    </script>

</body>

</html>

)=====";

//-----------------------------------------------------------------------------------------------------------------------------------

// Selection from Available Networks or Create new network
const char HTML_NetworkSelect[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Neo | Devices</title>

    <style type="text/css">
        h1 {
            display: block;
            color: black;
            font-family: 'Open Sans', sans-serif;
            font-size: clamp(23px, 10vw, 41.6px);
            line-height: 10px;
            margin-top: 0.75em;
            margin-bottom: 1em;
            margin-left: 0;
            margin-right: 0;
            font-weight: bold;
        }

        html {
            width: 100vw;
            text-align: center;
        }

        body {
            width: 100vw;
            margin: 0 auto;
        }

        p,
        label {
            display: block;
            color: black;
            font-family: 'Open Sans', sans-serif;
            font-size: clamp(15px, 4.5vw, 19px);
            line-height: 10px;
            margin-top: 1em;
            margin-bottom: 1em;
            margin-left: 0;
            margin-right: 0;
        }

        input {
            box-sizing: border-box;
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 28.5px;
            max-width: 370px;
            max-height: 30px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            font-size: clamp(14px, 3.5vw, 14.4px);
        }

        input[type=submit] {
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 30px;
            max-width: 370px;
            max-height: 40px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px #006600;
            border-radius: 5px;
            outline: none;
            background-color: #009933;
            color: white
        }

        input:focus[type=submit] {
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px #006600;
            border-radius: 5px;
            outline: none;
            box-shadow: 0 0 5px #006600;
            background-color: #00A435;
            color: white
        }

        select,
        option {
            box-sizing: border-box;
            width: 90vw;
            height: 5vh;
            min-width: 285px;
            min-height: 28.5px;
            max-width: 370px;
            max-height: 30px;
            overflow: auto;
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            font-size: clamp(14px, 3.5vw, 14.4px);
        }

        input:focus,
        select:focus,
        option:hover {
            margin: 0 auto;
            padding: 5px;
            border: 1px solid black;
            border-radius: 5px;
            outline: none;
            outline-color: #006600;
            box-shadow: 0 0 5px #009933;
        }
        option{
            text-align: center;
        }
    </style>

</head>

<body>
    
    <h1>NEO<span style="color: #009933"> LAN-NETWORK</span></h1>  
    <form>
        <label for="SSID">SELECT FROM EXISTING NETWORK:</label>
           <p>
                <select oninput="changeButton()" name="selectedNetwork" id="selNetwork">
                    <option class="option" value="NONE">NONE</option>
                </select>
            </p>
    </form>
    <p><input onClick="sendData(0)" id="proceed" style="background-color: #808080" name="selectBtn" type="submit" value="NEXT" /></p>
    <form>
        <label id="labelChange" for="SSID">CREATE NEW NETWORK:</label>
        <p><input oninput="addOptions()" maxlength = "20" class="input" id="newNetwork" name="networkBox" type="text" placeholder="Ex: LIVING ROOM"
                required="required" size="28" maxlength="30" />
        </p>
        
        <div id = "hidden">
            <label for="ApplInVoltage">Appliance Input Voltage:</label>
            <p><input class="input" id="ApplInVoltage" name="AIVBox" type="number" min="21"
                    max="300" step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
                    placeholder="Voltage in VAC (Optional)" size="28" maxlength="5"/>
            </p>
            
            <label for="UtilityCompany">Utility Company:</label>
            <select class="select" id="UtilityCompany" name="UtilityCoMenu">
                <option value="APS">APS (Arizona Public Service)</option>
                <option value="SRP">SRP (Salt River Project)</option>
            </select>

            <label for="RatePlan">Rate Plan:</label>
            <select class="select" id="RatePlan" name="RatePlanMenu">
                <option value="SCM">Saver Choice Max</option>
                <option value="E27">E-27 Customer Generation</option>
            </select>

            <label for="PlanPeakLimit">Peak Demand (Plan Limit):</label>
            <p><input class="input" id="PlanPeakLimit" name="PPDLBox" type="number" min="1" max="100"
                step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
                placeholder="Plan Limit in kW/Cycle" required="required" size="28" maxlength="5"/>
            </p>

            <label for="UserPeakTarget">Peak Demand (User Target):</label>
            <p><input class="input" id="UserPeakTarget" name="UPDTBox" type="number" min="1" max="100"
                step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
                placeholder="User Target in kW/Cycle" required="required" size="28" maxlength="5"/>
            </p>
        </div>
    </form>
    <p><input onClick="sendData(1)" id="createNetwork" name="networkBtn" type="submit" value="CREATE" /></p>
    <script>
        var selectedNetwork = "";
        var stringReceived = "";
        var pvoltage = "";
        var putilityCompany = "";
        var pratePlan = "";
        var pPlanPeakLimit = "";
        var pUserPeakTarget = "";
        
        var voltage = "";
        var utilityCompany = "";
        var ratePlan = "";
        var PlanPeakLimit = "";
        var UserPeakTarget = "";
        
        setInterval(function () {
            update();
        }, 2000);
        function changeButton(){
            if(document.getElementById("selNetwork").value != 'NONE'){
              document.getElementById("hidden").style.display = "none";
                document.getElementById("proceed").style.backgroundColor = "#009933";
                document.getElementById("proceed").disabled = false;
                document.getElementById("labelChange").innerHTML = "EDIT NETWORK PARAMETERS:";
                document.getElementById("newNetwork").value = document.getElementById("selNetwork").value;
                document.getElementById("newNetwork").disabled = true;
                document.getElementById("createNetwork").value = "EDIT";
                document.getElementById("createNetwork").onclick = function onclick(event){ editEnable()};
            }
            else{
                document.getElementById("labelChange").innerHTML = "CREATE NEW NETWORK:";
                document.getElementById("newNetwork").value = "";
                document.getElementById("newNetwork").disabled = false;
                document.getElementById("hidden").style.display = "none";
                document.getElementById("createNetwork").value = "CREATE";
                document.getElementById("ApplInVoltage").value = pvoltage;
                document.getElementById("UtilityCompany").value = putilityCompany;
                document.getElementById("RatePlan").value = pratePlan;
                document.getElementById("PlanPeakLimit").value = pPlanPeakLimit;
                document.getElementById("UserPeakTarget").value = pUserPeakTarget;
                document.getElementById("proceed").style.backgroundColor = "#808080";
                document.getElementById("proceed").disabled = true;
                document.getElementById("createNetwork").onclick = function onclick(event){ sendData(1)};
                document.getElementById("createNetwork").disabled = false;
            }
        }
        function update() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                  stringReceived = this.responseText;
                    if(document.getElementById("selNetwork").innerHTML != stringReceived){
                      document.getElementById("selNetwork").innerHTML = stringReceived;
                    }
                }
            };
            xhttp.open("GET", "getNetwork", true);
            xhttp.send();
            if(document.getElementsByClassName("option")[0].value == document.getElementById("selNetwork").value){
                for(var i = 0; i<document.getElementsByClassName("option").length; i++){
                    if(document.getElementsByClassName("option")[i].value == document.getElementById("newNetwork").value){
                        document.getElementById("newNetwork").value = "";
                    }
                }
            }
        }

        function sendData(){
            var tempString = "";
            if(arguments[0] == 0){
                tempString = "flag=0&selectedNetwork=" + document.getElementById("selNetwork").value;
            }
            else {
                if(arguments[0] == 1){
                    if(document.getElementById("newNetwork").value == ''){
                        alert("Please enter a valid network name!");
                        return;
                    }
                    tempString = "flag=1&";
                }
                else{
                    tempString = "flag=2&";
                }
                tempString += "networkBox="+ document.getElementById("newNetwork").value+
                             "&AIVBox="+ document.getElementById("ApplInVoltage").value+
                             "&UtilityCoMenu="+ document.getElementById("UtilityCompany").value+
                             "&RatePlanMenu="+ document.getElementById("RatePlan").value+
                             "&PPDLBox="+ document.getElementById("PlanPeakLimit").value+
                             "&UPDTBox="+ document.getElementById("UserPeakTarget").value;
                }
              if(confirm("Do you want to set the \"Current Device\" parameters same as the \"Network\" parameters?")){
                  tempString = "useNTWpar=1&" + tempString;
                }
                else{
                    tempString = "useNTWpar=0&" + tempString;
                }
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "selectedNetwork", true);
            xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
            xhr.onload = function () {
            // do something to response
              if(this.responseText == "SUCCESS"){
                window.location="http://" + window.location.host + "/LeafConfig";
              }
            }
            xhr.send(tempString);
        }

        function addOptions(){
            if(document.getElementById("newNetwork").value == ''){
                document.getElementById("hidden").style.display = "none";
            }
            else{
                document.getElementById("ApplInVoltage").value = "";
                document.getElementById("UtilityCompany").value = "";
                document.getElementById("RatePlan").value = "";
                document.getElementById("PlanPeakLimit").value = "";
                document.getElementById("UserPeakTarget").value = "";
                document.getElementById("hidden").style.display = "block";
            }
        }
        function parseParam(rspns){
            var output = ["","","","","",""]
            var commaIndex = [0,0,0,0,0,0];
            //console.log(rspns);
            commaIndex[0] = rspns.indexOf(',');
            for(var i = 1; i <6; i++){
                commaIndex[i] = rspns.indexOf(',',commaIndex[i-1]+1);
            }
            output[0] = rspns.substring(0,commaIndex[0]);
            output[1] = rspns.substring(commaIndex[0]+1,commaIndex[1]);
            output[2] = rspns.substring(commaIndex[1]+1,commaIndex[2]);
            output[3] = rspns.substring(commaIndex[2]+1,commaIndex[3]);
            output[4] = rspns.substring(commaIndex[3]+1,commaIndex[4]);
            output[5] = rspns.substring(commaIndex[4]+1,commaIndex[5]);
            return output;
        }

        function loadParams(){
            var xhr = new XMLHttpRequest();
            xhr.open("POST", "getNetwork", true);
            xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
            
            xhr.onload = function () {
            // do something to response
            var rspns = "";
            rspns = this.responseText;
            var received = parseParam(rspns);
            if(received[0] == document.getElementById("selNetwork").value){
                voltage = received[1];
                utilityCompany = received[2];
                ratePlan = received[3];
                PlanPeakLimit = received[4];
                UserPeakTarget = received[5]
            }
            else{
              voltage = "";
              utilityCompany = "";
              ratePlan = "";
              PlanPeakLimit = "";
              UserPeakTarget = "";
            }
            document.getElementById("ApplInVoltage").value = voltage;
            document.getElementById("UtilityCompany").value = utilityCompany;
            document.getElementById("RatePlan").value = ratePlan;
            document.getElementById("PlanPeakLimit").value = PlanPeakLimit;
            document.getElementById("UserPeakTarget").value = UserPeakTarget;
            };
            var network = document.getElementById("selNetwork").value;
            str = "NETWORK=" + network;
            xhr.send(str);
        }

        function editEnable(){
            let str = "";
            var xhr = new XMLHttpRequest();
            
            pvoltage = document.getElementById("ApplInVoltage").value;
            putilityCompany = document.getElementById("UtilityCompany").value;
            pratePlan = document.getElementById("RatePlan").value;
            pPlanPeakLimit = document.getElementById("PlanPeakLimit").value;
            pUserPeakTarget = document.getElementById("UserPeakTarget").value;

            loadParams();
            
            document.getElementById("labelChange").innerHTML = "EDIT EXISTING NETWORK:";
            document.getElementById("newNetwork").value = document.getElementById("selNetwork").value;
            document.getElementById("newNetwork").disabled = true;
            document.getElementById("hidden").style.display = "block";
            document.getElementById("createNetwork").onclick = function onclick(event){ sendData(2)};
            document.getElementById("createNetwork").value = "SAVE";
            document.getElementById("createNetwork").disabled = false;
        }
        
        function getSelectedNetwork(){
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                var rspns = this.responseText;
                var received = parseParam(rspns);
                document.getElementById("selNetwork").value = received[0];
                document.getElementById("ApplInVoltage").value = received[1];
                document.getElementById("UtilityCompany").value = received[2];
                document.getElementById("RatePlan").value = received[3];
                document.getElementById("PlanPeakLimit").value = received[4];
                document.getElementById("UserPeakTarget").value = received[5];
                changeButton();
            };
            xhttp.open("GET", "selectedNetwork", true);
            xhttp.send();
        }
        addOptions();
        changeButton();
        setTimeout(function(){getSelectedNetwork()},2500);
    </script>
</body>

</html>
)=====";

//---------------------------------------------------------------------------------------------------------------------------------

// Confirms Successful Input of WiFi Credentials & Clickthrough for FirstRun
const char HTML_WiFiConfirmFR[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en-us">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">

<title>Neo | WiFi Config</title>

<style type="text/css">
/* Import Open Sans Font From Google Servers */
/*@import url('https://fonts.googleapis.com/css2?family=Open+Sans:wght@400;700&display=swap');*/
/* Font Styling */
h2 {display: block; color:black; font-family:'Open Sans', sans-serif; font-size: clamp(23px, 10vw, 41.6px); line-height:10px; margin-top: 0.75em; margin-bottom: 1em; margin-left: 0; margin-right: 0; font-weight: bold;}
p, label {display: block; color:black; font-family:'Open Sans', sans-serif; font-size: clamp(15px, 4.5vw, 19px); line-height:10px; margin-top: 1em; margin-bottom: 1em; margin-left: 0; margin-right: 0;}
/* Change Text Selection Color */
::selection, ::-moz-selection {color: white; background: #009933;}
/* Wrapper Styling */
html {width: 100vw; text-align: center;}
body {width: 100vw; margin: 0 auto;}
div {overflow: hidden;}
form {}
/* Object Styling */
input {box-sizing: border-box; width: 90vw; height: 5vh; min-width: 285px; min-height: 28.5px; max-width: 370px; max-height: 30px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; font-size: clamp(14px, 3.5vw, 14.4px);}
input[type=submit] {width: 90vw; height: 5vh; min-width: 285px; min-height: 30px; max-width: 370px; max-height: 40px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px #006600; border-radius: 5px; outline: none; background-color: #009933; color: white}
input:focus[type=submit] {overflow: auto; margin: 0 auto; padding: 5px; border: 1px #006600; border-radius: 5px; outline: none; box-shadow: 0 0 5px #006600; background-color: #00A435; color: white}
select, option {box-sizing: border-box; width: 90vw; height: 5vh; min-width: 285px; min-height: 28.5px; max-width: 370px; max-height: 30px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; font-size: clamp(14px, 3.5vw, 14.4px);}
input:focus, select:focus, option:hover  { margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; outline-color: #006600; box-shadow: 0 0 5px #009933;}

</style>
</head>

<body>
<form action="/LeafConfig"> 
  <h2>NEO<span style="color: #009933"> WIFI CONFIG</span></h2><br>
  <label for="Submit">Credentials Saved! Continue Setup?</label><br>
  <p><input id="Submit" name="SubmitBtn" type="submit" value="Continue"/></p>
</form></body></html>


)=====";

//-----------------------------------------------------------------------------------------------------------------------------------

// For inputting device configuration data
const char HTML_LeafConfig[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en-us">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Neo | Leaf Config</title>
<link rel="icon" href="data:,">

<style type="text/css">
/* Import Open Sans Font From Google Servers */
@import url('https://fonts.googleapis.com/css2?family=Open+Sans:wght@400;700&display=swap');
/* Font Styling */
h2 {display: block; color:black; font-family:'Open Sans', sans-serif; font-size: clamp(23px, 10vw, 41.6px); line-height:10px; margin-top: 0.75em; margin-bottom: 1em; margin-left: 0; margin-right: 0; font-weight: bold;}
p, label {display: block; color:black; font-family:'Open Sans', sans-serif; font-size: clamp(15px, 4.5vw, 19px); line-height:10px; margin-top: 1em; margin-bottom: 1em; margin-left: 0; margin-right: 0;}
/* Change Text Selection Color */
::selection, ::-moz-selection {color: white; background: #009933;}
/* Wrapper Styling */
html {width: 100vw; text-align: center;}
body {width: 100vw; margin: 0 auto;}
div {overflow: hidden;}
form {}
/* Object Styling */
input {box-sizing: border-box; width: 90vw; height: 5vh; min-width: 285px; min-height: 28.5px; max-width: 370px; max-height: 30px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; font-size: clamp(14px, 3.5vw, 14.4px);}
input[type=submit] {width: 90vw; height: 5vh; min-width: 285px; min-height: 30px; max-width: 370px; max-height: 40px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px #006600; border-radius: 5px; outline: none; background-color: #009933; color: white}
input:focus[type=submit] {overflow: auto; margin: 0 auto; padding: 5px; border: 1px #006600; border-radius: 5px; outline: none; box-shadow: 0 0 5px #006600; background-color: #00A435; color: white}
select, option {box-sizing: border-box; width: 90vw; height: 5vh; min-width: 285px; min-height: 28.5px; max-width: 370px; max-height: 30px; overflow: auto; margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; font-size: clamp(14px, 3.5vw, 14.4px);}
input:focus, select:focus, option:hover  { margin: 0 auto; padding: 5px; border: 1px solid black; border-radius: 5px; outline: none; outline-color: #006600; box-shadow: 0 0 5px #009933;}

</style>
</head>

<body>
<form action="/LeafConfig" method="POST">
  <h2>NEO<span style="color: #009933"> LEAF CONFIG</span></h2>
    
    <label for="AttAppliance" title="Please select the device or appliance that Neo is wired to">Attached Appliance:</label>
  <select class="select" id="AttAppliance" name="AttApplianceMenu" title="Please select the device or appliance that Neo is wired to">
    <option value="AC1" title="The primary AC in the residence">Air Conditioner (Primary)</option>
    <option value="AC2" title="The secondary AC in the residence">Air Conditioner (Secondary)</option>
        <option value="ACO" title="Any other AC system or similar device">Air Conditioner (Other)</option>
        <option value="SC1" title="The primary swamp cooler in the residence">Swamp Cooler (Primary)</option>
        <option value="SC2" title="The secondary swamp cooler in the residence">Swamp Cooler (Secondary)</option>
        <option value="SCO" title="Any other swamp cooler or similar device">Swamp Cooler (Other)</option>
        <option value="HEF" title="An electrically powered furnace system">Electric Furnace</option>
        <option value="HEP" title="An electrically powered heat pump system">Electric Heat Pump</option>
        <option value="HEO" title="Any other electric heater system or similar device">Electric Heater (Other)</option>
        <option value="WHT" title="An electrically powered water heater tank">Electric Water Heater Tank</option>
        <option value="AER" title="A combination electric stove and oven">All-In-One Electric Range</option>
        <option value="ECS" title="An electrically powered cooktop stove (NO OVEN)">Electric Cooktop Stove</option>
        <option value="OVE" title="An electrically powered oven only">Electric Oven Only</option>
        <option value="DRY">Electric Dryer</option>
        <option value="OA1">Other Appliance 1</option>
        <option value="OA2">Other Appliance 2</option>
  </select>
    
    <label for="ApplPriority">Appliance Usage Priority:</label>
  <select class="select" id="ApplPriority" name="ApplPriorityMenu">
    <option value="1">1 (Absolutely Need)</option>
    <option value="2">2 (Need More Than Want)</option>
        <option value="3">3 (Neutral)</option>
        <option value="4">4 (Want More Than Need)</option>
        <option value="5">5 (Can Live Without)</option>
  </select>
    <br><br>
    <br><br>
    <h2 style="font-size: clamp(23px, 10vw, 30px);">COMMON<span style="color: #009933"> PARAMETERS</span></h2>
  <div id="hidden">
     <label for="ApplInVoltage">Appliance Input Voltage:</label>
  <p><input class="input" id="ApplInVoltage" name="AIVBox" type="number" min="21"
    max="300" step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
    placeholder="Voltage in VAC (Optional)" size="28" maxlength="5"/></p>
    
    
    <label for="UtilityCompany">Utility Company:</label>
  <select class="select" id="UtilityCompany" name="UtilityCoMenu">
    <option value="APS">APS (Arizona Public Service)</option>
    <option value="SRP">SRP (Salt River Project)</option>
  </select>
    
    <label for="RatePlan">Rate Plan:</label>
  <select class="select" id="RatePlan" name="RatePlanMenu">
    <option value="SCM">Saver Choice Max</option>
    <option value="E27">E-27 Customer Generation</option>
  </select>
    
    <label for="PlanPeakLimit">Peak Demand (Plan Limit):</label>
  <p><input class="input" id="PlanPeakLimit" name="PPDLBox" type="number" min="1" max="100"
    step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
    placeholder="Plan Limit in kW/Cycle" required="required" size="28" maxlength="5"/></p>
    
  <label for="UserPeakTarget">Peak Demand (User Target):</label>
  <p><input class="input" id="UserPeakTarget" name="UPDTBox" type="number" min="1" max="100"
    step="0.1" oninput="this.value = this.value.replace(/[^0-9.]/g, '').replace(/(\..*?)\..*/g, '$1');"
    placeholder="User Target in kW/Cycle" required="required" size="28" maxlength="5"/></p>
  </div>
    <p><input id="Submit" name="SubmitBtn" type="submit" value="Submit" /></p>
</form>
<form action="/reset" method="GET">
  <input id="Reset" name="resetBtn" type="submit" value="Reset Device" />
</form>
<script>
    var receivedString = "";
    
    function parseParam(rspns){
            var output = ["","","","","","",""]
            var commaIndex = [0,0,0,0,0,0,0];
            console.log(rspns);
            commaIndex[0] = rspns.indexOf(',');
            for(var i = 1; i <7; i++){
                commaIndex[i] = rspns.indexOf(',',commaIndex[i-1]+1);
            }
            output[0] = rspns.substring(0,commaIndex[0]);
            output[1] = rspns.substring(commaIndex[0]+1,commaIndex[1]);
            output[2] = rspns.substring(commaIndex[1]+1,commaIndex[2]);
            output[3] = rspns.substring(commaIndex[2]+1,commaIndex[3]);
            output[4] = rspns.substring(commaIndex[3]+1,commaIndex[4]);
            output[5] = rspns.substring(commaIndex[4]+1,commaIndex[5]);
            output[6] = rspns.substring(commaIndex[5]+1,commaIndex[6]);
            return output;
        }

    function getParams(){
      var xhttp = new XMLHttpRequest();

      xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        var rspns = this.responseText;
        var received = parseParam(rspns);
        document.getElementById("AttAppliance").value = received[0];
        document.getElementById("ApplInVoltage").value = received[1];
        document.getElementById("UtilityCompany").value = received[2];
        document.getElementById("RatePlan").value = received[3];
        document.getElementById("PlanPeakLimit").value = received[4];
        document.getElementById("UserPeakTarget").value = received[5];
        document.getElementById("ApplPriority").value = received[6];
      }
    };
      xhttp.open("GET", "/getParams", true);
      xhttp.send();
    }
    setTimeout(function(){getParams();},1000);

</script>
</body>
</html>
)=====";

//-----------------------------------------------------------------------------------------------------------------------------------
// For showing the connected NEO LEAF Devices
const char devices_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Neo | Devices</title>

    <style type="text/css">
        h1 {display: block; color:black; font-family:'Open Sans', sans-serif; font-size: clamp(23px, 10vw, 41.6px); line-height:10px; margin-top: 0.75em; margin-bottom: 1em; margin-left: 0; margin-right: 0; font-weight: bold;}
        html {width: 100vw; text-align: center;}
        body {width: 100vw; margin: 0 auto;}
        ul{ list-style: none; margin-left:-2em; text-align: center; border:}
        li{margin:20px;} //border-style: solid; border-radius: 1rem; }
    </style>

</head>
<body>
    <h1>NEO<span style="color: #009933"> DEVICES</span></h1>
    <div>
        <h2>CONNECTED DEVICES</h2>
        <div id="connected">
            <ul>
                <li style="list-style: none">NONE</li>
            </ul>
        </div>
    </div>
    <script>
        setInterval(function(){
            updateConnected();
        }, 2000);

        function updateConnected(){
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("connected").innerHTML =
                    this.responseText;
                }
            };
        xhttp.open("GET", "readDevices", true);
        xhttp.send();
        }
    </script>
</body>
</html>
)=====";
