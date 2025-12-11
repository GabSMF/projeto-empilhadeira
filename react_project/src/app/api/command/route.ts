import { NextRequest, NextResponse } from "next/server";

export async function POST(request: NextRequest) {
  try {
    const body = await request.json();
    const { ip, command } = body;

    if (!ip || !command) {
      return NextResponse.json(
        { error: "IP e comando são obrigatórios" },
        { status: 400 }
      );
    }

    const url = `http://${ip}/command`;
    console.log(`Sending command "${command}" to ${url}`);

    const controller = new AbortController();
    const timeoutId = setTimeout(() => controller.abort(), 5000); // 5s timeout

    try {
      // Envia o comando como texto puro (mais comum em dispositivos embarcados)
      const response = await fetch(url, {
        method: "POST",
        headers: {
          "Content-Type": "text/plain",
        },
        body: command,
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      const responseText = await response.text();

      if (response.ok) {
        return NextResponse.json({
          success: true,
          message: `Comando ${command} enviado com sucesso`,
          response: responseText,
        });
      } else {
        return NextResponse.json(
          {
            success: false,
            error: `Erro do servidor: ${response.status}`,
            response: responseText,
          },
          { status: response.status }
        );
      }
    } catch (fetchError) {
      clearTimeout(timeoutId);
      
      const err = fetchError as NodeJS.ErrnoException;
      let errorMessage = "Erro desconhecido";

      if (err.name === "AbortError") {
        errorMessage = "Timeout - dispositivo não respondeu em 5s";
      } else if (err.cause && typeof err.cause === "object") {
        const cause = err.cause as NodeJS.ErrnoException;
        
        switch (cause.code) {
          case "EHOSTUNREACH":
            errorMessage = `Host inacessível (${ip}) - Verifique se o dispositivo está ligado e na mesma rede`;
            break;
          case "ECONNREFUSED":
            errorMessage = `Conexão recusada (${ip}) - Verifique se o servidor está rodando na porta correta`;
            break;
          case "ETIMEDOUT":
            errorMessage = `Timeout de conexão (${ip}) - Dispositivo não respondeu`;
            break;
          case "ENOTFOUND":
            errorMessage = `Endereço não encontrado (${ip}) - Verifique o IP`;
            break;
          default:
            errorMessage = `Erro de rede: ${cause.code || cause.message}`;
        }
      }

      console.error("Erro ao enviar comando:", errorMessage);
      
      return NextResponse.json(
        {
          success: false,
          error: errorMessage,
        },
        { status: 500 }
      );
    }
  } catch (error) {
    console.error("Erro ao processar requisição:", error);
    return NextResponse.json(
      {
        success: false,
        error: error instanceof Error ? error.message : "Erro desconhecido",
      },
      { status: 500 }
    );
  }
}
