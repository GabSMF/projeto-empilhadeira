import cv2
from pupil_apriltags import Detector
import numpy as np

# --- Configuração ---
ESP_IP = "192.168.0.107:81"  
STREAM_URL = f"http://{ESP_IP}/stream"

# --- 1. CONFIGURAÇÃO DE POSE (PREENCHA AQUI) ---

# Meça sua tag (a parte preta, em metros)
TAG_SIZE_METERS = 0.113 # Exemplo: 5cm

#define TAG_SIZE 0.036

# --- 2. DADOS DE CALIBRAÇÃO (PLACEHOLDER) ---
#    Estes são VALORES DE TESTE (para uma câmera 800x600)
#    Você DEVE substituí-los por valores de uma calibração real!
#    Sem calibração, a pose será imprecisa.

CAMERA_MATRIX = np.array([
    [462.53494149,   0.0,       249.20712346],  # fx, 0, cx
    [  0.0,        457.9657483, 184.23721053],  # 0, fy, cy
    [  0.0,          0.0,         1.0       ]
], dtype=np.float32)

# Coeficientes de Distorção (k1, k2, p1, p2, k3)
DIST_COEFFS = np.array([0.09291399, -0.07356924, 0.00380924, -0.00405076, -0.76082793])

# Pegamos os 4 parâmetros intrínsecos que o detector precisa
# (fx, fy, cx, cy)
CAMERA_PARAMS = [
    CAMERA_MATRIX[0, 0], # fx
    CAMERA_MATRIX[1, 1], # fy
    CAMERA_MATRIX[0, 2], # cx
    CAMERA_MATRIX[1, 2]  # cy
]
# --- Fim da Configuração de Pose ---


# --- Inicialização ---
print(f"Tentando conectar ao stream em: {STREAM_URL}")

at_detector = Detector(
   families="tag36h11",
   nthreads=1,
   quad_decimate=1.0,
   quad_sigma=0.0,
   refine_edges=1,
   decode_sharpening=0.25,
   debug=0
)

try:
    cap = cv2.VideoCapture(STREAM_URL)
except Exception as e:
    print(f"Erro ao abrir o VideoCapture: {e}")
    exit()

if not cap.isOpened():
    print("Erro: Não foi possível abrir o stream de vídeo.")
    exit()

print("Conectado! Pressione 'q' na janela de vídeo para sair.")

# --- Loop Principal de Processamento ---
while True:
    try:
        ret, frame = cap.read()
        if not ret or frame is None:
            print("Perda de stream...")
            cap.release()
            cap = cv2.VideoCapture(STREAM_URL)
            cv2.waitKey(1000)
            continue

        # --- 1. CORREÇÃO DA IMAGEM ---
        # Remove a distorção "olho de peixe" usando os dados de calibração
        # Se os DIST_COEFFS forem zeros, esta etapa não fará quase nada
        frame_undistorted = cv2.undistort(frame, CAMERA_MATRIX, DIST_COEFFS)

        # Converte a imagem *corrigida* para escala de cinza
        gray = cv2.cvtColor(frame_undistorted, cv2.COLOR_BGR2GRAY)
        
        # --- 2. DETECÇÃO COM POSE ---
        # Agora passamos os parâmetros da câmera e o tamanho da tag
        results = at_detector.detect(
            gray, 
            estimate_tag_pose=True, # Habilita a estimativa de pose
            camera_params=CAMERA_PARAMS,
            tag_size=TAG_SIZE_METERS
        )
        
        print(f"Tags detectadas: {len(results)}")

        # --- 3. DESENHANDO OS RESULTADOS ---
        for r in results:
            # Extrai os cantos (para desenhar a caixa 2D)
            (ptA, ptB, ptC, ptD) = r.corners.astype(int)
            cv2.polylines(frame, [r.corners.astype(int)], True, (0, 255, 0), 2)
            
            # --- DADOS DA POSE ---
            # pose_R é a matriz de rotação (orientação)
            # pose_t é o vetor de translação (posição X, Y, Z)
            pose_t = r.pose_t
            pose_R = r.pose_R

            # Imprime o vetor de translação (posição) no console
            # X: (positivo para a direita)
            # Y: (positivo para baixo)
            # Z: (positivo para frente, distância da câmera)
            print(f"  - Tag ID: {r.tag_id}")
            print(f"    Posição (X, Y, Z) em metros: {pose_t.flatten()}")

            # --- 4. DESENHAR OS EIXOS 3D ---
            # Desenha os eixos 3D na imagem *original* (frame)
            # A função drawFrameAxes precisa da Matriz da Câmera e Coefs. de Distorção
            # O tamanho (0.1) é o comprimento dos eixos desenhados (10cm)
            cv2.drawFrameAxes(frame, CAMERA_MATRIX, DIST_COEFFS, pose_R, pose_t, 0.1)

            # Escreve o ID da tag
            cv2.putText(frame, str(r.tag_id), (ptA[0], ptA[1] - 15),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

        # Exibe o quadro (o original, com os desenhos por cima)
        cv2.imshow("ESP32-CAM Feed com AprilTags", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    except Exception as e:
        print(f"Ocorreu um erro no loop: {e}")
        break

# --- Limpeza ---
print("Encerrando o script...")
cap.release()
cv2.destroyAllWindows()
