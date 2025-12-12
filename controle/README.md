# Controle do Robô

Este diretório contém os códigos de teste e desenvolvimento da lógica de movimentação do robô via Serial.

## Arquivos

### `controle_sem_pid`
Implementação funcional do controle básico do robô via comandos na porta Serial.
- **Comandos**: `F` (Frente), `B` (Trás), `L` (Esquerda), `R` (Direita), `U` (Subir Lift), `D` (Descer Lift), `P`/`S` (Parar).
- **Lógica**: Controle direto via PWM sem malha fechada.
- **Status**: **Funcional**. Base para a versão WiFi.

### `controle_com_pid`
Tentativa de implementação de controle PID para garantir movimento retilíneo.
- **Objetivo**: Usar encoders para equalizar a velocidade das rodas.
- **Resultado**: **Descartado**. Os encoders disponíveis mostraram-se muito imprecisos para um ajuste fino eficaz.
