<script lang="ts">
    import { model } from "$lib/stores";
    import { goto } from "$app/navigation";
    import SSHModel from "./SSHModel.svelte";
    import RAG from "./RAG.svelte";

    const startChat = async() => {
        if (!$model) return

        const response = await fetch("/reload", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({model: $model})
        })
        if (response.ok) goto("/chat")
    }
</script>

<nav class="sidebar">
    <div class="sidebar-header">
        <div class="header-title-row">
            <span class="header-dot"></span>
            <span class="header-title">CLUSTER CONFIG</span>
        </div>
        <p class="header-sub">Distributed Inference</p>
    </div>

    <div class="sidebar-body">
        <div class="section">
            <label class="section-label" for="model-select">Model</label>
            <div class="select-wrapper">
                <select id="model-select" bind:value={$model}>
                    <option value="" disabled>Select a model…</option>
                    <option value="ggml-org/gemma-3-1b-it-GGUF">Gemma 3 - 1b</option>
                    <option value="bartowski/Llama-3.2-1B-Instruct-GGUF">Llama 3.2 - 1B</option>
                    <option value="bartowski/Qwen2.5-3B-GGUF">Qwen 2.5 - 3B</option>
                    <option value="bartowski/DeepSeek-R1-Distill-Llama-8B-GGUF">DeepSeek R1 - 8B</option>
                </select>
                <svg class="select-chevron" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <polyline points="6 9 12 15 18 9"></polyline>
                </svg>
            </div>
            {#if $model}
                <div class="model-badge">
                    <span class="badge-pulse"></span>
                    <span class="badge-text">{$model}</span>
                </div>
            {/if}
        </div>

        <div class="divider"></div>

        <div class="section">
            <p class="section-label">Nodes</p>
            <SSHModel />
        </div>
        
        <div class="divider"></div>

        <div class="section">
            <p class="section-label">Knowledge Base</p>
            <RAG />
        </div>
        
    </div>

    <div class="sidebar-footer">
        <button class="start-btn" onclick={startChat} disabled={!$model}>
            <svg viewBox="0 0 24 24" fill="currentColor" width="13" height="13">
                <polygon points="5 3 19 12 5 21 5 3"></polygon>
            </svg>
            Launch Chat
        </button>
    </div>
</nav>

<style>
    .sidebar {
        flex-grow: 1;
        position: absolute;
        top: 0;
        right: 0;
        width: 300px;
        height: 100vh;
        background-color: var(--bg-surface);
        border-left: 1px solid var(--color-border);
        display: flex;
        flex-direction: column;
        overflow: hidden;
    }

    /* Header */
    .sidebar-header {
        padding: 24px 20px 18px;
        border-bottom: 1px solid var(--color-border);
        flex-shrink: 0;
    }

    .header-title-row {
        display: flex;
        align-items: center;
        gap: 8px;
        margin-bottom: 4px;
    }

    .header-dot {
        width: 7px;
        height: 7px;
        background-color: var(--color-accent-green);
        border-radius: 50%;
        box-shadow: var(--shadow-glow);
        animation: pulse 2.5s ease-in-out infinite;
        flex-shrink: 0;
    }

    .header-title {
        font-family: var(--font-mono);
        font-size: 11px;
        font-weight: 700;
        letter-spacing: 0.12em;
        color: var(--text-primary);
    }

    .header-sub {
        margin: 0 0 0 15px;
        font-family: var(--font-sans);
        font-size: 12px;
        color: var(--text-muted);
    }

    /* Body */
    .sidebar-body {
        flex: 1;
        overflow-y: auto;
        padding: 20px;
        display: flex;
        flex-direction: column;
        gap: 0;
    }

    .section {
        display: flex;
        flex-direction: column;
        gap: 10px;
        padding: 4px 0 20px;
    }

    .section-label {
        font-family: var(--font-sans);
        font-size: 11px;
        font-weight: 600;
        color: var(--text-muted);
        text-transform: uppercase;
        letter-spacing: 0.08em;
        margin: 0;
    }

    .divider {
        height: 1px;
        background-color: var(--color-border);
        margin: 0 0 20px;
    }

    /* Select */
    .select-wrapper {
        position: relative;
    }

    select {
        width: 100%;
        appearance: none;
        -webkit-appearance: none;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        color: var(--text-primary);
        font-family: var(--font-sans);
        font-size: 13px;
        padding: 10px 36px 10px 12px;
        border-radius: var(--border-radius-md);
        cursor: pointer;
        outline: none;
        transition: border-color 0.2s ease;
    }

    select:focus {
        border-color: var(--text-muted);
    }

    select option {
        background-color: var(--bg-surface-elevated);
        color: var(--text-primary);
    }

    .select-chevron {
        position: absolute;
        right: 10px;
        top: 50%;
        transform: translateY(-50%);
        width: 14px;
        height: 14px;
        color: var(--text-muted);
        pointer-events: none;
    }

    /* Selected model badge */
    .model-badge {
        display: flex;
        align-items: center;
        gap: 8px;
        background-color: rgba(74, 222, 128, 0.06);
        border: 1px solid rgba(74, 222, 128, 0.2);
        border-radius: var(--border-radius-md);
        padding: 8px 12px;
    }

    .badge-pulse {
        width: 6px;
        height: 6px;
        background-color: var(--color-accent-green);
        border-radius: 50%;
        box-shadow: var(--shadow-glow);
        flex-shrink: 0;
        animation: pulse 2.5s ease-in-out infinite;
    }

    .badge-text {
        font-family: var(--font-mono);
        font-size: 10px;
        color: var(--color-accent-green);
        overflow: hidden;
        text-overflow: ellipsis;
        white-space: nowrap;
        letter-spacing: 0.02em;
    }

    /* Footer */
    .sidebar-footer {
        padding: 16px 20px 20px;
        border-top: 1px solid var(--color-border);
        flex-shrink: 0;
    }

    .start-btn {
        width: 100%;
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 8px;
        background-color: var(--color-accent-green);
        color: #000;
        font-family: var(--font-sans);
        font-size: 13px;
        font-weight: 600;
        padding: 11px 20px;
        border: none;
        border-radius: var(--border-radius-md);
        cursor: pointer;
        letter-spacing: 0.03em;
        box-shadow: var(--shadow-glow);
        transition: background-color 0.2s ease, box-shadow 0.2s ease, transform 0.15s ease;
    }

    .start-btn:hover:not(:disabled) {
        background-color: #6ee8a0;
        box-shadow: 0 0 22px rgba(74, 222, 128, 0.5);
        transform: translateY(-1px);
    }

    .start-btn:active:not(:disabled) {
        transform: translateY(0);
    }

    .start-btn:disabled {
        background-color: var(--bg-surface-elevated);
        color: var(--text-muted);
        box-shadow: none;
        cursor: not-allowed;
    }

    @keyframes pulse {
        0%, 100% { opacity: 1; }
        50% { opacity: 0.35; }
    }
</style>
