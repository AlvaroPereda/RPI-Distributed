<script lang="ts">
    let { handleNewPrompt} = $props();
    let prompt = $state("");


    const handleKeydown = (e: KeyboardEvent) => {
        if (e.key === "Enter" && !e.shiftKey) {
            e.preventDefault()
            sendPrompt()
        }
    }

    const sendPrompt = () => {
        if (!prompt.trim()) return
        handleNewPrompt(prompt)
        prompt = ""
    }

</script>

<div class="input-wrapper">
    <div class="input-container">
        <textarea
            bind:value={prompt}
            onkeydown={handleKeydown}
            placeholder="Send a message…"
            rows="1"
        ></textarea>
        <button type="button" aria-label="Send message" onclick={sendPrompt} class="send-btn">
            <svg viewBox="0 0 24 24" fill="currentColor" width="16" height="16">
                <path d="M2.01 21L23 12 2.01 3 2 10l15 2-15 2z"></path>
            </svg>
        </button>
    </div>
    <p class="input-hint">Press <kbd>Enter</kbd> to send · <kbd>Shift+Enter</kbd> for new line</p>
</div>

<style>
    .input-wrapper {
        position: fixed;
        bottom: 0;
        left: 0;
        right: 0;
        display: flex;
        flex-direction: column;
        align-items: center;
        padding: 16px 24px 20px;
        background: linear-gradient(to top, var(--bg-deep) 70%, transparent);
        z-index: 50;
    }

    .input-container {
        display: flex;
        align-items: flex-end;
        gap: 10px;
        width: 100%;
        max-width: 760px;
        background-color: var(--bg-surface);
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-lg);
        padding: 12px 12px 12px 18px;
        transition: border-color 0.2s ease;
    }

    .input-container:focus-within {
        border-color: var(--text-muted);
    }

    textarea {
        flex: 1;
        background: transparent;
        border: none;
        outline: none;
        color: var(--text-primary);
        font-family: var(--font-sans);
        font-size: 14px;
        line-height: 1.6;
        resize: none;
        min-height: 24px;
        max-height: 180px;
        overflow-y: auto;
        padding: 0;
    }

    textarea::placeholder {
        color: var(--text-muted);
    }

    .send-btn {
        flex-shrink: 0;
        display: flex;
        align-items: center;
        justify-content: center;
        width: 36px;
        height: 36px;
        background-color: var(--color-accent-green);
        color: #000;
        border: none;
        border-radius: var(--border-radius-md);
        cursor: pointer;
        transition: box-shadow 0.2s ease, transform 0.15s ease, background-color 0.2s ease;
        box-shadow: var(--shadow-glow);
    }

    .send-btn:hover {
        background-color: #6ee8a0;
        box-shadow: 0 0 22px rgba(74, 222, 128, 0.5);
        transform: scale(1.05);
    }

    .send-btn:active {
        transform: scale(0.96);
    }

    .input-hint {
        margin: 8px 0 0;
        font-family: var(--font-sans);
        font-size: 11px;
        color: var(--text-muted);
    }

    .input-hint kbd {
        font-family: var(--font-mono);
        font-size: 10px;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        border-radius: 4px;
        padding: 1px 5px;
        color: var(--text-secondary);
    }
</style>
