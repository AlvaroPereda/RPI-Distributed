<script lang="ts">
    import type { LlamaAPI, Metrics, Prompt } from "$lib/types";

    import Chat from "./chat.svelte";
    import Header from "./header.svelte";
    import Input from "./input.svelte";

    let history: Prompt[] = $state([])

    async function handleNewPrompt(prompt: string) {
        history = [...history, { role: 'user', content: prompt }];
        history = [...history, { role: 'assistant', content: '' }];

        const systemInstruction = {
            role: "system", 
            content: "Eres un asistente. Solo responde en texto plano. No uses JSON."
        };

        const message = [systemInstruction, ...history];

        try {
            const response = await fetch('http://localhost:8081/v1/chat/completions', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    messages: message,
                    model: "model.gguf",
                    temperature: 0.7,
                    stream: true // Va pasando información cuando la tiene no espera a que este toda la respuesa completa
                })
            })
            if (!response.body) throw new Error("No response body");

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
            console.error("Error conectando con llama-server:", error);
            history.push({role: "assistant", content: "error conectando con el servidor"})
        }
    }

</script>
<main>
    <Header />
    <Chat history={history}/>
    <Input onPrompt={handleNewPrompt}/>
</main>

<style>
    main {
        max-width: 800px; margin: 0 auto; padding: 20px; font-family: sans-serif;
    }
</style>