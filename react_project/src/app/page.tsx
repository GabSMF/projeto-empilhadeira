"use client";

import { useState, useCallback } from "react";

type Direction = "F" | "B" | "U" | "D" | "R" | "L" | "S";

interface PWMSettings {
  F: number;
  B: number;
  U: number;
  D: number;
  R: number;
  L: number;
}

export default function Home() {
  const [ip, setIp] = useState("192.168.1.1");
  const [status, setStatus] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [pwm, setPwm] = useState<PWMSettings>({
    F: 100,
    B: 100,
    U: 100,
    D: 100,
    R: 100,
    L: 100,
  });

  const sendCommand = useCallback(
    async (direction: Direction) => {
      setIsLoading(true);
      setStatus(null);

      const command = direction === "S" ? "S" : `${direction}${pwm[direction]}`;
      console.log(`Sending command: ${command} to http://${ip}/command`);

      try {
        // Requisição direta para o ESP32 com body em texto raw
        const response = await fetch(`http://${ip}/command`, {
          method: "POST",
          headers: {
            "Content-Type": "text/plain",
          },
          body: command,
        });

        if (response.ok) {
          const responseText = await response.text();
          setStatus(
            `✓ Comando enviado: ${command}${
              responseText ? ` - ${responseText}` : ""
            }`
          );
        } else {
          console.error(
            `Error sending command: ${response.status} ${response.statusText}`
          );
          setStatus(`✗ Erro: ${response.status} ${response.statusText}`);
        }
      } catch (error) {
        console.error("Error sending command:", error);
        setStatus(
          `✗ Erro de conexão: ${
            error instanceof Error ? error.message : "Desconhecido"
          }`
        );
      } finally {
        setIsLoading(false);
      }
    },
    [ip, pwm]
  );

  const handlePwmChange = (direction: keyof PWMSettings, value: string) => {
    const numValue = Math.min(255, Math.max(0, parseInt(value) || 0));
    setPwm((prev) => ({ ...prev, [direction]: numValue }));
  };

  const DirectionButton = ({
    direction,
    label,
    icon,
    className = "",
  }: {
    direction: Direction;
    label: string;
    icon: string;
    className?: string;
  }) => (
    <button
      onClick={() => sendCommand(direction)}
      disabled={isLoading}
      className={`
        group relative flex flex-col items-center justify-center
        w-20 h-20 rounded-2xl
        bg-gradient-to-b from-zinc-700 to-zinc-800
        border border-zinc-600/50
        shadow-lg shadow-black/30
        hover:from-zinc-600 hover:to-zinc-700
        active:from-zinc-800 active:to-zinc-900
        active:shadow-inner
        disabled:opacity-50 disabled:cursor-not-allowed
        transition-all duration-150
        ${className}
      `}
    >
      <span className="text-2xl mb-1 group-hover:scale-110 transition-transform">
        {icon}
      </span>
      <span className="text-xs font-medium text-zinc-300 uppercase tracking-wider">
        {label}
      </span>
      <div className="absolute inset-0 rounded-2xl bg-amber-500/0 group-hover:bg-amber-500/10 transition-colors" />
    </button>
  );

  const PWMInput = ({
    direction,
    label,
  }: {
    direction: keyof PWMSettings;
    label: string;
  }) => (
    <div className="flex items-center gap-3 bg-zinc-800/50 rounded-xl px-4 py-3 border border-zinc-700/50">
      <span className="text-sm font-mono text-amber-400 w-8">{direction}</span>
      <span className="text-sm text-zinc-400 flex-1">{label}</span>
      <input
        type="number"
        min="0"
        max="255"
        value={pwm[direction]}
        onChange={(e) => handlePwmChange(direction, e.target.value)}
        className="
          w-20 px-3 py-2 
          bg-zinc-900 border border-zinc-600 rounded-lg
          text-center font-mono text-amber-300
          focus:outline-none focus:ring-2 focus:ring-amber-500/50 focus:border-amber-500
          transition-all
        "
      />
    </div>
  );

  return (
    <div className="min-h-screen bg-gradient-to-br from-zinc-950 via-zinc-900 to-zinc-950">
      {/* Background Pattern */}
      <div
        className="fixed inset-0 opacity-[0.02]"
        style={{
          backgroundImage: `url("data:image/svg+xml,%3Csvg width='60' height='60' viewBox='0 0 60 60' xmlns='http://www.w3.org/2000/svg'%3E%3Cg fill='none' fill-rule='evenodd'%3E%3Cg fill='%23ffffff' fill-opacity='1'%3E%3Cpath d='M36 34v-4h-2v4h-4v2h4v4h2v-4h4v-2h-4zm0-30V0h-2v4h-4v2h4v4h2V6h4V4h-4zM6 34v-4H4v4H0v2h4v4h2v-4h4v-2H6zM6 4V0H4v4H0v2h4v4h2V6h4V4H6z'/%3E%3C/g%3E%3C/g%3E%3C/svg%3E")`,
        }}
      />

      <div className="relative z-10 container mx-auto px-4 py-8 max-w-4xl">
        {/* Header */}
        <header className="text-center mb-10">
          <div className="inline-flex items-center gap-3 mb-4">
            <span className="text-4xl">🏗️</span>
            <h1 className="text-4xl font-bold bg-gradient-to-r from-amber-400 via-orange-400 to-amber-500 bg-clip-text text-transparent">
              Controle da Empilhadeira
            </h1>
          </div>
          <p className="text-zinc-500 text-sm tracking-wide uppercase">
            Sistema de Controle Remoto
          </p>
        </header>

        {/* IP Input Section */}
        <section className="mb-8 bg-zinc-900/50 backdrop-blur rounded-2xl p-6 border border-zinc-800">
          <label className="block text-sm font-medium text-zinc-400 mb-3 uppercase tracking-wider">
            Endereço IP do Controlador
          </label>
          <div className="flex gap-3">
            <div className="relative flex-1">
              <span className="absolute left-4 top-1/2 -translate-y-1/2 text-zinc-500 font-mono text-sm">
                http://
              </span>
              <input
                type="text"
                value={ip}
                onChange={(e) => setIp(e.target.value)}
                placeholder="192.168.1.1:80"
                className="
                  w-full pl-16 pr-4 py-4
                  bg-zinc-800 border border-zinc-700 rounded-xl
                  text-zinc-100 font-mono text-lg
                  placeholder:text-zinc-600
                  focus:outline-none focus:ring-2 focus:ring-amber-500/50 focus:border-amber-500
                  transition-all
                "
              />
            </div>
            <span className="flex items-center text-zinc-500 font-mono text-sm">
              /command
            </span>
          </div>
          <p className="text-xs text-zinc-600 mt-2">
            Formato: IP ou IP:porta (ex: 192.168.0.104 ou 192.168.0.104:8080)
          </p>
        </section>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {/* Control Pad */}
          <section className="bg-zinc-900/50 backdrop-blur rounded-2xl p-6 border border-zinc-800">
            <h2 className="text-sm font-medium text-zinc-400 mb-6 uppercase tracking-wider text-center">
              Controles de Direção
            </h2>

            {/* Direction Grid */}
            <div className="flex flex-col items-center gap-4">
              {/* Up */}
              <DirectionButton direction="U" label="Cima" icon="⬆️" />

              {/* Middle Row: Left, Forward, Stop, Backward, Right */}
              <div className="flex items-center gap-4">
                <DirectionButton direction="L" label="Esq" icon="⬅️" />
                <DirectionButton direction="F" label="Frente" icon="🔼" />

                {/* Stop Button - Special */}
                <button
                  onClick={() => sendCommand("S")}
                  disabled={isLoading}
                  className="
                    group relative flex flex-col items-center justify-center
                    w-24 h-24 rounded-full
                    bg-gradient-to-b from-red-600 to-red-700
                    border-4 border-red-500/50
                    shadow-lg shadow-red-900/50
                    hover:from-red-500 hover:to-red-600
                    active:from-red-700 active:to-red-800
                    active:shadow-inner
                    disabled:opacity-50 disabled:cursor-not-allowed
                    transition-all duration-150
                  "
                >
                  <span className="text-3xl mb-1">🛑</span>
                  <span className="text-xs font-bold text-white uppercase tracking-wider">
                    Stop
                  </span>
                </button>

                <DirectionButton direction="B" label="Trás" icon="🔽" />
                <DirectionButton direction="R" label="Dir" icon="➡️" />
              </div>

              {/* Down */}
              <DirectionButton direction="D" label="Baixo" icon="⬇️" />
            </div>

            {/* Keyboard Hint */}
            <p className="text-center text-zinc-600 text-xs mt-6">
              Use os botões para controlar a empilhadeira
            </p>
          </section>

          {/* PWM Settings */}
          <section className="bg-zinc-900/50 backdrop-blur rounded-2xl p-6 border border-zinc-800">
            <h2 className="text-sm font-medium text-zinc-400 mb-6 uppercase tracking-wider text-center">
              Configurações PWM (0-255)
            </h2>

            <div className="space-y-3">
              <PWMInput direction="F" label="Frente" />
              <PWMInput direction="B" label="Trás" />
              <PWMInput direction="U" label="Cima" />
              <PWMInput direction="D" label="Baixo" />
              <PWMInput direction="R" label="Direita" />
              <PWMInput direction="L" label="Esquerda" />
            </div>

            {/* Quick PWM Presets */}
            <div className="mt-6 pt-4 border-t border-zinc-800">
              <p className="text-xs text-zinc-500 mb-3 uppercase tracking-wider">
                Presets Rápidos
              </p>
              <div className="flex gap-2">
                {[50, 100, 150, 200, 255].map((preset) => (
                  <button
                    key={preset}
                    onClick={() =>
                      setPwm({
                        F: preset,
                        B: preset,
                        U: preset,
                        D: preset,
                        R: preset,
                        L: preset,
                      })
                    }
                    className="
                      flex-1 py-2 rounded-lg
                      bg-zinc-800 border border-zinc-700
                      text-zinc-400 text-sm font-mono
                      hover:bg-amber-500/20 hover:text-amber-400 hover:border-amber-500/50
                      transition-all
                    "
                  >
                    {preset}
                  </button>
                ))}
              </div>
            </div>
          </section>
        </div>

        {/* Status Bar */}
        <div
          className={`
            mt-6 p-4 rounded-xl border transition-all duration-300
            ${
              status?.startsWith("✓")
                ? "bg-emerald-500/10 border-emerald-500/30 text-emerald-400"
                : status?.startsWith("✗")
                ? "bg-red-500/10 border-red-500/30 text-red-400"
                : "bg-zinc-800/50 border-zinc-700/50 text-zinc-500"
            }
          `}
        >
          <div className="flex items-center gap-3">
            <div
              className={`
                w-3 h-3 rounded-full
                ${
                  isLoading
                    ? "bg-amber-500 animate-pulse"
                    : status?.startsWith("✓")
                    ? "bg-emerald-500"
                    : status?.startsWith("✗")
                    ? "bg-red-500"
                    : "bg-zinc-600"
                }
              `}
            />
            <span className="font-mono text-sm">
              {isLoading
                ? "Enviando comando..."
                : status || "Aguardando comando..."}
            </span>
          </div>
        </div>

        {/* Footer */}
        <footer className="mt-8 text-center">
          <p className="text-zinc-600 text-xs">
            PUC-Rio • Projeto Empilhadeira • Robótica
          </p>
        </footer>
      </div>
    </div>
  );
}
