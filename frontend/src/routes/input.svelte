<script lang="ts">
    let { onPrompt } = $props()
    let prompt = $state("")
    let textareaRef: HTMLTextAreaElement | null = null;

    function sendPrompt() {
        if (!prompt.trim()) return;
        onPrompt(prompt);
        prompt = "";
        if (textareaRef) {
            textareaRef.style.height = 'auto';
        }
    }

    function autoResize() {
        if (!textareaRef) return;
        textareaRef.style.height = 'auto'; 
        textareaRef.style.height = textareaRef.scrollHeight + 'px';
    }

    function handleKeydown(e: KeyboardEvent) {
        if (e.key === "Enter" && !e.shiftKey) {
            e.preventDefault();
            sendPrompt();
        }
    }
</script>

<div class="input-container-fixed">
    <div class="input-wrapper">
        
        <div class="input-bar">
            <button type="button" class="icon-btn" title="">
                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="10"></circle>
                    <path d="M12 8v8M8 12h8"></path>
                </svg>
            </button>

            <textarea
                bind:this={textareaRef}
                bind:value={prompt}
                placeholder="Ask anything"
                rows="1"
                class="auto-grow-textarea"
                oninput={autoResize}
                onkeydown={handleKeydown}
            ></textarea>

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

<style>
    .input-container-fixed {
        position: fixed;
        bottom: 0;
        left: 0;
        width: 100%;
        background-color: black;
        z-index: 1000;
        background: linear-gradient(to top, #000 80%, transparent 100%);
        padding-top: 20px; 
    }
    
    .input-wrapper {
        max-width: 900px;
        margin: 0 auto;
        padding: 0 20px 20px 20px;
        box-sizing: border-box;
    }

    .input-bar {
        background-color: #1e1e1e;
        border-radius: 24px;
        display: flex;
        align-items: flex-end; 
        padding: 12px 15px;
        gap: 10px;
        border: 1px solid transparent;
        transition: border-color 0.2s;
        min-height: 24px;
    }

    .input-bar:focus-within {
        background-color: #2d2d2d;
        border-color: #444;
    }

    .auto-grow-textarea {
        flex: 1;
        background: transparent;
        border: none;
        outline: none;
        color: #E3E3E3;
        font-size: 1rem;
        font-family: inherit;
        resize: none;
        max-height: 150px;
        overflow-y: auto;
        padding: 0;
        margin: 0;
        line-height: 1.5;
        white-space: pre-wrap; 
        word-wrap: break-word; 
    }

    .auto-grow-textarea::-webkit-scrollbar {
        width: 4px;
    }
    .auto-grow-textarea::-webkit-scrollbar-thumb {
        background-color: #555;
        border-radius: 2px;
    }

    .icon-btn, .send-btn {
        background: transparent;
        border: none;
        cursor: pointer;
        color: #888;
        padding: 5px;
        padding-bottom: 2px;
        display: flex;
        align-items: center;
        justify-content: center;
        height: 32px;
        width: 32px;
    }
    
    .send-btn svg { width: 20px; height: 20px; }
    .send-btn.active { color: #fff; }
    .icon-btn:hover { color: #E3E3E3; }
</style>