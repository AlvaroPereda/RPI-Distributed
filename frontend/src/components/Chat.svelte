<script lang="ts">
    import type { Prompt } from "$lib/types";

    let { history }: { history: Prompt[] }= $props()
</script>

<div id="chat-container">
    <div class="chat-area">
        {#each history as {role, content, metrics}}
            <div class="message-wrapper {role}">
                {#if role === 'user'}
                    <div class="message user-bubble">
                        <p class="message-text">{content}</p>
                    </div>
                {:else}
                    <div class="message assistant-bubble">
                        {#if content === ''}
                            <div class="typing-indicator">
                                <span></span>
                                <span></span>
                                <span></span>
                            </div>
                        {:else}
                            <p class="message-text">{content}</p>
                            {#if metrics}
                                <div class="metrics-bar">
                                    <span class="metric-item">
                                        <span class="metric-label">speed</span>
                                        <span class="metric-value">{metrics.predicted_per_second} <span class="metric-unit">tok/s</span></span>
                                    </span>
                                    <span class="metric-separator">·</span>
                                    <span class="metric-item">
                                        <span class="metric-label">time</span>
                                        <span class="metric-value">{metrics.predicted_ms}<span class="metric-unit">s</span></span>
                                    </span>
                                    <span class="metric-separator">·</span>
                                    <span class="metric-item">
                                        <span class="metric-label">tokens</span>
                                        <span class="metric-value">{metrics.predicted_n}</span>
                                    </span>
                                </div>
                            {/if}
                        {/if}
                    </div>
                {/if}
            </div>
        {/each}
    </div>
</div>

<style>
    #chat-container {
        display: flex;
        justify-content: center;
        padding: 80px 24px 160px;
        min-height: 100vh;
        background-color: var(--bg-deep);
    }

    .chat-area {
        display: flex;
        flex-direction: column;
        width: 100%;
        max-width: 760px;
        gap: 20px;
    }

    .message-wrapper {
        display: flex;
        flex-direction: column;
    }

    .message-wrapper.user {
        align-items: flex-end;
    }

    .message-wrapper.assistant {
        align-items: flex-start;
    }

    .message {
        padding: 14px 18px;
        border-radius: var(--border-radius-md);
        max-width: 75%;
        word-break: break-word;
    }

    .user-bubble {
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        border-bottom-right-radius: 3px;
    }

    .assistant-bubble {
        background-color: var(--bg-surface);
        border: 1px solid var(--color-border);
        border-bottom-left-radius: 3px;
        min-width: 60px;
    }

    .message-text {
        margin: 0;
        color: var(--text-primary);
        font-family: var(--font-sans);
        font-size: 14px;
        line-height: 1.7;
        white-space: pre-wrap;
    }

    /* Typing indicator */
    .typing-indicator {
        display: flex;
        align-items: center;
        gap: 5px;
        padding: 4px 2px;
    }

    .typing-indicator span {
        display: block;
        width: 6px;
        height: 6px;
        background-color: var(--color-accent-green);
        border-radius: 50%;
        box-shadow: var(--shadow-glow);
        animation: bounce 1.2s ease-in-out infinite;
    }

    .typing-indicator span:nth-child(2) {
        animation-delay: 0.2s;
    }

    .typing-indicator span:nth-child(3) {
        animation-delay: 0.4s;
    }

    @keyframes bounce {
        0%, 60%, 100% { transform: translateY(0); opacity: 0.5; }
        30% { transform: translateY(-5px); opacity: 1; }
    }

    /* Metrics */
    .metrics-bar {
        display: flex;
        align-items: center;
        gap: 8px;
        margin-top: 10px;
        padding-top: 10px;
        border-top: 1px solid var(--color-border);
        flex-wrap: wrap;
    }

    .metric-item {
        display: flex;
        align-items: baseline;
        gap: 4px;
    }

    .metric-label {
        font-family: var(--font-mono);
        font-size: 10px;
        color: var(--text-muted);
        text-transform: uppercase;
        letter-spacing: 0.05em;
    }

    .metric-value {
        font-family: var(--font-mono);
        font-size: 11px;
        color: var(--color-accent-green);
        font-weight: 700;
    }

    .metric-unit {
        font-size: 9px;
        color: var(--text-muted);
        font-weight: 400;
    }

    .metric-separator {
        color: var(--color-border);
        font-size: 14px;
        line-height: 1;
    }
</style>
