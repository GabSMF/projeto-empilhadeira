import cv2
from pupil_apriltags import Detector
import numpy as np

# --- Configuração ---
# 1. COLOQUE O IP DA SUA ESP32-CAM AQUI
#    (Você pode ver o IP no monitor serial do Arduino IDE)
ESP_IP = "192.168.0.107:81"  

# 2. Este é o endpoint de stream padrão do exemplo "CameraWebServer"
#    Se a sua ESP32 estiver na porta 81, mude para:
#    STREAM_URL = f"http://{ESP_IP}:81/stream"
STREAM_URL = f"http://{ESP_IP}/stream"

# --- Inicialização ---

print(f"Tentando conectar ao stream em: {STREAM_URL}")

# Inicializa o detector de AprilTag
at_detector = Detector(
   families="tag36h11",
   nthreads=1,
   quad_decimate=1.0,
   quad_sigma=0.0,
   refine_edges=1,
   decode_sharpening=0.25,
   debug=0
)

# Inicializa a captura de vídeo do OpenCV
# O OpenCV faz todo o trabalho de decodificação do stream HTTP
try:
    cap = cv2.VideoCapture(STREAM_URL)
except Exception as e:
    print(f"Erro ao abrir o VideoCapture: {e}")
    exit()

if not cap.isOpened():
    print("Erro: Não foi possível abrir o stream de vídeo.")
    print("Verifique:")
    print("1. O IP da ESP32 está correto?")
    print("2. A ESP32 está na mesma rede Wi-Fi?")
    print("3. O endpoint '/stream' está correto?")
    exit()

print("Conectado! Pressione 'q' na janela de vídeo para sair.")

# --- Loop Principal de Processamento ---
while True:
    try:
        # 1. Lê um quadro (frame) do stream
        ret, frame = cap.read()

        if not ret or frame is None:
            print("Perda de stream. Tentando reconectar...")
            cap.release()
            cap = cv2.VideoCapture(STREAM_URL)
            cv2.waitKey(1000) # Espera 1 segundo antes de tentar novamente
            continue

        # 2. Processa a imagem para AprilTags
        # Converte para escala de cinza (necessário para o detector)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        # Detecta as tags
        results = at_detector.detect(gray)
        
        # 3. Desenha as detecções no quadro
        for r in results:
            # Extrai os 4 cantos da tag
            (ptA, ptB, ptC, ptD) = r.corners.astype(int)
            
            # Desenha a caixa delimitadora
            cv2.polylines(frame, [r.corners.astype(int)], True, (0, 255, 0), 2)
            
            # Desenha o centro (círculo)
            (cX, cY) = r.center.astype(int)
            cv2.circle(frame, (cX, cY), 5, (0, 0, 255), -1)
            
            # Escreve o ID da tag
            tag_id = r.tag_id
            cv2.putText(frame, str(tag_id), (ptA[0], ptA[1] - 15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

        # 4. Exibe o quadro
        cv2.imshow("ESP32-CAM Feed com AprilTags", frame)

        # Pressione 'q' para sair
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    except Exception as e:
        print(f"Ocorreu um erro no loop: {e}")
        break

# --- Limpeza ---
print("Encerrando o script...")
cap.release()
cv2.destroyAllWindows()
