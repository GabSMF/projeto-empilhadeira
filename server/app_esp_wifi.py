from flask import Flask, jsonify

app = Flask(__name__)

# comando atual (um char: 'F','B','L','R','S')
current_cmd = "S"  # S = Stop

@app.route("/f")
def set_forward():
    global current_cmd
    current_cmd = "F"
    return jsonify({"status": "ok", "cmd": current_cmd})

@app.route("/b")
def set_backward():
    global current_cmd
    current_cmd = "B"
    return jsonify({"status": "ok", "cmd": current_cmd})

@app.route("/l")
def set_left():
    global current_cmd
    current_cmd = "L"
    return jsonify({"status": "ok", "cmd": current_cmd})

@app.route("/r")
def set_right():
    global current_cmd
    current_cmd = "R"
    return jsonify({"status": "ok", "cmd": current_cmd})

@app.route("/stop")
def set_stop():
    global current_cmd
    current_cmd = "S"
    return jsonify({"status": "ok", "cmd": current_cmd})

@app.route("/cmd")
def get_cmd():
    """
    Endpoint que o ESP32 chama para pegar o comando atual.
    Aqui eu devolvo só o caractere, pra ficar fácil de tratar no ESP32.
    """
    global current_cmd
    cmd = current_cmd
    # Se quiser, pode "consumir" o comando:
    # current_cmd = "S"
    return cmd  # text/plain: "F" ou "B" etc.

if __name__ == "__main__":
    # host="0.0.0.0" pra outros dispositivos da rede enxergarem
    app.run(host="0.0.0.0", port=5000, debug=True)
