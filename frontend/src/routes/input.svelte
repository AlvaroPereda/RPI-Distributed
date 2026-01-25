<script lang="ts">
    let { onPrompt } = $props();
    let prompt = $state("");
    let textareaRef: HTMLTextAreaElement | null = null;
    let selectedModel = $state("ggml-org/gemma-3-1b-it-GGUF");
    let device = $state("");

    function sendPrompt() {
        if (!prompt.trim()) return;
        onPrompt(prompt);
        prompt = "";
        resetSize();
    }

    function autoResize() {
        if (!textareaRef) return;
        textareaRef.style.height = 'auto';
        textareaRef.style.height = textareaRef.scrollHeight + 'px';
    }

    function resetSize() {
        if (textareaRef) {
            textareaRef.style.height = 'auto';
        }
    }

    function handleKeydown(e: KeyboardEvent) {
        if (e.key === "Enter" && !e.shiftKey) {
            e.preventDefault();
            sendPrompt();
        }
    }

    const changeModel = async() => {
        const result = await fetch("/reload", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({model: selectedModel})
        })
        if (result.ok) {
            console.log("Modelo cambiado correctamente");
        } else {
            console.error("Error al cambiar el modelo");
        }
    }

    const addDevice = async() => {
        const result = await fetch("devices", {
            method: "POST",
            headers: {"Content-Type": "aplicacion/json"},
            body: JSON.stringify({device: device})
        })
        if(result.ok) {
            console.log("Dispositvo añadido correctamente");
        } else {
            console.log("Error al añadir dispositivo");
        }
    }


</script>

<div class="input-container-fixed">
    <div class="input-wrapper">
        <div class="input-bar">
            
            <div class="input-area-row">
                <textarea
                    bind:this={textareaRef}
                    bind:value={prompt}
                    placeholder="Ask anything"
                    rows="1"
                    class="auto-grow-textarea"
                    oninput={autoResize}
                    onkeydown={handleKeydown}
                ></textarea>
            </div>

            <div class="input-actions-row">
                
                <div class="left-actions">
                    <button type="button" class="icon-btn" aria-label="Attachments">
                        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                            <circle cx="12" cy="12" r="10"></circle>
                            <path d="M12 8v8M8 12h8"></path>
                        </svg>
                    </button>
                </div>

                <div class="right-actions">
                    
                    <div class="select-wrapper">
                        <select class="model-select" title="Model Selection" onchange={changeModel} bind:value={selectedModel}> 
                            <option>ggml-org/gemma-3-1b-it-GGUF</option>
                            <option>bartowski/Llama-3.2-1B-Instruct-GGUF</option>
                        </select>
                    </div>

                    <div>
                        <input 
                            type="text" 
                            placeholder="Device name"
                            bind:value={device}
                        />
                        <button type="button" onclick={addDevice}>Add device</button>
                    </div>

                    <button 
                        title="Send"
                        type="button" 
                        class="send-btn {prompt.length > 0 ? 'active' : ''}" 
                        onclick={sendPrompt}
                    >
                        <svg viewBox="0 0 24 24" fill="currentColor">
                            <path d="M2.01 21L23 12 2.01 3 2 10l15 2-15 2z"></path>
                        </svg>
                    </button>
                </div>
            </div>

        </div>
    </div>
</div>

<style>
    .input-container-fixed {
        position: fixed;
        bottom: 0;
        left: 0;
        width: 100%;
        z-index: 1000;
        background: linear-gradient(to top, #000 70%, transparent 100%);
        padding-top: 40px; 
        padding-bottom: 20px;
    }
    
    .input-wrapper {
        max-width: 800px;
        margin: 0 auto;
        padding: 0 1rem;
    }

    .input-bar {
        background-color: #1e1e1e;
        border-radius: 26px;
        display: flex;
        flex-direction: column;
        padding: 12px 16px;
        border: 1px solid #333;
        transition: border-color 0.2s, background-color 0.2s;
        box-shadow: 0 4px 6px rgba(0,0,0,0.1);
    }

    .input-bar:focus-within {
        border-color: #555;
        background-color: #252525;
    }

    .input-area-row {
        width: 100%;
        margin-bottom: 8px;
    }

    .auto-grow-textarea {
        width: 100%;
        background: transparent;
        border: none;
        outline: none;
        color: #ECECEC;
        font-size: 1rem;
        font-family: inherit;
        resize: none;
        max-height: 200px;
        overflow-y: auto;
        padding: 0;
        margin: 0;
        line-height: 1.5;
    }

    .auto-grow-textarea::-webkit-scrollbar { width: 4px; }
    .auto-grow-textarea::-webkit-scrollbar-thumb { background-color: #555; border-radius: 2px; }

    .input-actions-row {
        display: flex;
        justify-content: space-between;
        align-items: center;
        height: 32px;
    }

    .left-actions, .right-actions {
        display: flex;
        align-items: center;
        gap: 8px;
    }

    .icon-btn, .send-btn {
        background: transparent;
        border: none;
        cursor: pointer;
        color: #888;
        padding: 6px;
        border-radius: 50%;
        display: flex;
        align-items: center;
        justify-content: center;
        transition: background-color 0.2s, color 0.2s;
    }

    .icon-btn:hover {
        background-color: rgba(255,255,255,0.1);
        color: #E3E3E3;
    }

    .send-btn svg { width: 18px; height: 18px; }
    
    .send-btn {
        background-color: #333;
        color: #666;
    }

    .send-btn.active {
        background-color: #ECECEC;
        color: #000;
    }
    
    .send-btn.active:hover {
        background-color: #fff;
    }

    .select-wrapper {
        position: relative;
    }

    .model-select {
        appearance: none;
        background-color: transparent;
        border: none;
        color: #888;
        font-size: 0.85rem;
        font-weight: 500;
        cursor: pointer;
        padding: 4px 8px;
        border-radius: 6px;
        transition: color 0.2s, background-color 0.2s;
        outline: none;
    }

    .model-select:hover {
        color: #ECECEC;
        background-color: rgba(255,255,255,0.05);
    }

    .model-select option {
        background-color: #1e1e1e;
        color: #ECECEC;
    }
</style>