from flask import Flask,request,jsonify
from datetime import datetime

app=Flask(__name__)

latest_event={
    "event":"NONE",
    "device":"none",
    "time":"none"
}

@app.route("/event",methods=["POST"])
def event():
    global latest_event

    data=request.json

    latest_event={
        "event":data.get("event"),
        "device":data.get("device"),
        "time":str(datetime.now())
    }

    print("================")
    print("Receive Event:")
    print(latest_event)
    print("================")

    return jsonify({
        "status":"ok"
    })


@app.route("/status",methods=["GET"])
def status():
    return jsonify(latest_event)


@app.route("/",methods=["GET"])
def index():
    return """
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>ESP32 Sensor Monitor</title>
<style>
body{
    font-family:Arial;
    text-align:center;
    margin-top:80px;
}
.box{
    width:400px;
    margin:auto;
    padding:30px;
    border:1px solid #ccc;
    border-radius:10px;
}
.event{
    font-size:30px;
    font-weight:bold;
}
</style>
</head>

<body>

<div class="box">
<h1>Home Sensor Node</h1>

<h2>Current Event</h2>

<div class="event" id="event">
NONE
</div>

<p>
Device:
<span id="device">
none
</span>
</p>

<p>
Time:
<span id="time">
none
</span>
</p>

</div>


<script>
function updateStatus()
{
    fetch("/status")
    .then(response=>response.json())
    .then(data=>{
        document.getElementById("event").innerHTML=data.event;
        document.getElementById("device").innerHTML=data.device;
        document.getElementById("time").innerHTML=data.time;
    });
}

setInterval(updateStatus,1000);

updateStatus();

</script>

</body>
</html>
"""


app.run(
    host="0.0.0.0",
    port=5000
)