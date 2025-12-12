# Controlador por WiFi

Este diretório contém a versão final e testes do controlador do robô acessível via rede WiFi.

## Arquivos

### `controlador_por_wifi.ino`
Versão principal e funcional do firmware do robô.
- **Funcionalidade**: Cria um WebServer no ESP32 que aceita comandos POST.
- **Integração**: Combina a lógica de `controle_sem_pid` com o servidor web.
- **Uso**: Recebe comandos (ex: `F200`, `U255`) na rota `/command` e aciona os motores.

### `controla_wifi_testando_RPM_TABLE`
Tentativa de calibração manual via tabela de RPM.
- **Objetivo**: Mapear valores de PWM para corrigir diferenças entre os motores e andar em linha reta.
- **Resultado**: **Descartado**. A calibração feita em bancada não se sustentou no chão devido à variação de carga e atrito com o peso da estrutura.
