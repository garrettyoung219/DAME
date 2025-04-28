
# DAME: TARS Voice Interaction Server

### Senior Design Project | Department of Electrical Engineering | University of Notre Dame

## Overview

This is the **server-side implementation** for the ND EE Senior Design 2025 Project DAME: a recreation of **TARS**, the tactical robot from *Interstellar*. This server acts as the backend for an ESP32-based client, enabling real-time, bidirectional communication using OpenAI's **Realtime API** over WebSockets.

The server handles:

- Streaming **audio and text** conversations.
- Executing **tools/functions** requested by OpenAI (e.g., math, search).
- **Recording, processing, and transmitting** microphone data.
- **Broadcasting** OpenAI's real-time responses back to the ESP32.

---

## Architecture

```
[ESP32 Microcontroller]  ↔ (WebSocket) ↔  [DAME Server]  ↔  [OpenAI Realtime API]
```

- The ESP32 connects to this server over a **WebSocket** on `/device`.
- This server connects to OpenAI's **Realtime WebSocket API**.
- Server manages two-way communication:
  - Microphone/audio data (binary audio buffers)
  - Textual messages (transcriptions, commands, tool results)

---

## Core Technologies

| Technology          | Purpose                                                                      |
| ------------------- | ---------------------------------------------------------------------------- |
| TypeScript          | Strong typing and modern JavaScript features.                                |
| Hono                | Lightweight web server framework for Node.js.                                |
| WebSocket           | Bi-directional communication between ESP32 and server, and server to OpenAI. |
| LangChain           | Tool execution framework for interacting with OpenAI function-calling.       |
| OpenAI Realtime API | Streaming text + audio responses from GPT-4o.                                |

---

## Project Structure

```
/src
  |-- index.ts             # Main server entrypoint
  |-- prompt.ts            # TARS roleplaying instructions and conversation prompt
  |-- tools.ts             # Custom functions/tools exposed to OpenAI
  /lib
    |-- agent.ts           # Manages OpenAI Realtime WebSocket session and I/O
    |-- audio.ts           # Handles audio buffering, resampling, processing
    |-- connections.ts     # WebSocket connection to OpenAI API
    |-- executor.ts        # Executes LangChain tools when called by OpenAI
    |-- utils.ts           # Utility functions for streams, audio encoding, etc.
```

---

## Server Details

### `/device` WebSocket Endpoint

- **Protocol:** WebSocket
- **Purpose:** Handles communication with ESP32 clients.
- **Audio Handling:**
  - Incoming binary audio chunks are assembled and sent to OpenAI.
  - Outgoing audio responses from OpenAI are chunked and broadcast back to all connected clients.
- **Text Handling:**
  - Incoming text (if any) can be processed and sent to OpenAI.
  - Outgoing transcripts and tool outputs are forwarded back to ESP32.

### OpenAI WebSocket Connection

- **Model Used:** `gpt-4o-realtime-preview`
- **Realtime Stream Types:**
  - `response.audio.delta` (Audio output)
  - `response.audio_transcript.delta` (Transcribed text output)
  - `response.function_call_arguments.done` (Tool/function call request)
- **Voice:** `ash` (Natural-sounding real-time voice generation)
- **Audio format:** 16-bit PCM at 24kHz (optimized for ESP32 streaming)

### TARS Personality (Prompting)

- **TARS Instructions:**
  - Dry, witty, direct style (75% humor setting)
  - No unnecessary backstory explanations.
  - Mission-first, unflinching.
- **Natural Conversation Goal:**
  - Spoken interaction feels fluid, not scripted.

### Supported Tools

| Tool Name      | Description                                    |
| -------------- | ---------------------------------------------- |
| `add`          | Adds two numbers (for testing function calls). |
| `tavilySearch` | Real-time internet search via Tavily API.      |

When a tool is called by OpenAI, it is:

- Executed locally on the server.
- Result streamed back to OpenAI to inform TARS' response.

---

## Setup Instructions

1. **Install Node.js (>=18)**

2. **Clone this repository**

3. **Install dependencies**

```bash
npm install
```

4. **Set environment variables**
   Create a `.env` file with your OpenAI API key:

```bash
OPENAI_API_KEY=your-openai-api-key-here
```

5. **Run the server**

```bash
npm run dev
```

The server will start on **port 8888**.

6. **ESP32** connects to `ws://your-server-ip:8888/device`

---

## Audio Handling Details

- Incoming ESP32 audio data:

  - Buffered safely (preventing overflows).
  - Resampled if needed.
  - Converted into Base64-encoded PCM16 to match OpenAI input format.

- Outgoing OpenAI audio:

  - Chunked into small 1024-byte packets.
  - Sent immediately back to ESP32.

- Text data:

  - Buffered separately from audio.
  - Text output delayed slightly after audio to prioritize streaming smoothness.

---

## Key Design Highlights

- **Fully Real-Time:**

  - Simultaneous audio + text processing for natural conversation.

- **Tool Integration:**

  - OpenAI can request real-world actions (math, search).

- **Optimized for Embedded:**

  - Audio chunking, lower sample rate, and buffering logic are tuned for ESP32 capabilities.

- **TARS Roleplaying:**

  - Personality-driven prompts make interactions entertaining and believable.

---

## Group Members

- Garrett Young
- Matthew Sims
- Jack McGarrity
- Xander Steele
- Jack Corrao

---

## Acknowledgements

- [LangChain](https://js.langchain.com/)
- [Hono Web Framework](https://hono.dev/)
- [OpenAI Realtime API](https://platform.openai.com/docs/realtime)
- [Tavily Search API](https://tavily.com)

### Special Credit

This project is an adaptation of the architecture developed by [Fabrikapp Agency](https://github.com/FabrikappAgency/esp32-realtime-voice-assistant).
We extend sincere thanks for providing the foundation and inspiration for our server design.
