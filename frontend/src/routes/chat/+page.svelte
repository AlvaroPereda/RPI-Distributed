<script lang="ts">
    import type { LlamaAPI, Metrics, Prompt } from "$lib/types";
    import Chat from "../../components/Chat.svelte";
    import Header from "../../components/Header.svelte";
    import Input from "../../components/Input.svelte";

    let history: Prompt[] = $state([])

    const handleNewPrompt = async(prompt: string) => {
        history = [...history, { role: 'user', content: prompt }]
        history = [...history, { role: 'assistant', content: '' }];

        const systemInstruction:Prompt = {
            role: "system",
            content: "Eres un asistente. Solo responde en texto plano. No uses JSON."
        }

        const message = [systemInstruction, ...history]

        try {
            const response = await fetch("/chat/completions", {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    prompt: prompt,
                    use_rag: true
                })
            })
            if (response.status !== 200) throw new Error("Error en la respuesta del servidor.")
            if (!response.body) throw new Error("No response body")

            const reader = response.body.getReader(); // Abre un canal para toda la información
            const decoder = new TextDecoder();
            let aiMessageIndex = history.length - 1;

            while (true) {
                const { done, value } = await reader.read();
                if (done) break;

                const chunk = decoder.decode(value);
                const lines = chunk.split('\n');

                for (const line of lines) {
                    if (line.startsWith('data: ')) {
                        const dataStr = line.replace('data: ', '').trim();
                        if (dataStr === '[DONE]') break;
                        
                        try {
                            const data:LlamaAPI = JSON.parse(dataStr);
                            const content = data.choices[0].delta.content || "";

                            let newMetrics: Metrics | undefined = undefined;
                            
                            if (data.timings) {
                                newMetrics = {
                                    predicted_per_second: Number((data.timings.predicted_per_second).toFixed(2)),
                                    predicted_ms: Number((data.timings.predicted_ms / 1000).toFixed(2)),
                                    predicted_n: data.timings.predicted_n
                                };
                            }
                            
                            // Esto se hace así ya que como se manda por partes la respuesta no se puede hacer un push normal, ya que quedaría en distintos mensajes del chat
                            const newHistory = [...history];
                            const currentMsg = newHistory[aiMessageIndex];
                            currentMsg.content += content;
                            if (newMetrics) 
                                currentMsg.metrics = newMetrics;
                            history = newHistory; 

                        } catch (e) {
                            console.error("Error parseando JSON", e);
                        }
                    }
                }
            }
        } catch (error) {
            console.error(`Error con la solicitud: ${error}`)
            history.push({role: 'assistant', content: "Ha ocurrido un error al procesar tu solicitud."})
        }
    }

</script>
<main class="chat-page">
    <Header />
    <Chat {history} />
    <Input {handleNewPrompt} />
</main>

<style>
    .chat-page {
        background-color: var(--bg-deep);
        min-height: 100vh;
    }
</style>