<script lang="ts">
    import type { Prompt } from "$lib/types";

    type ChatProps = {
        history: Prompt[]
    }

    let { history }: ChatProps= $props()

</script>

<div class="chat-area">
    {#each history as {role, content, metrics}}
        <div class="message {role}">
            {content}
        </div>
        {#if metrics}
            <div class="metrics-container">
                <div class="metric-pill" title="Tokens generados">
                    <svg class="icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M4 7V4h16v3M9 20h6M12 4v16" stroke-linecap="round" stroke-linejoin="round"/>
                    </svg>
                    <span class="value">{metrics.predicted_n}</span>
                    <span class="unit">tokens</span>
                </div>

                <div class="metric-pill" title="Tiempo de evaluación">
                    <svg class="icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"></circle>
                        <polyline points="12 6 12 12 16 14"></polyline>
                    </svg>
                    <span class="value">{metrics.predicted_ms}</span>
                    <span class="unit">s</span>
                </div>

                <div class="metric-pill" title="Velocidad de generación">
                    <svg class="icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="m12 14 4-4"></path>
                        <path d="M3.34 19a10 10 0 1 1 17.32 0"></path>
                    </svg>
                    <span class="value">{metrics.predicted_per_second}</span>
                    <span class="unit">tokens/s</span>
                </div>
            </div>
        {/if}
    {/each}
    <div class="spacer"></div>
</div>

<style>
    .chat-area {
        height: 100%;
        min-height: 400px;
        overflow-y: auto;
        padding: 20px;
        background-color: transparent; 
        display: flex;
        flex-direction: column;
        gap: 16px;
        font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
        padding-bottom: 0;
    }
    .spacer {
        height: 120px;
        width: 100%;
        flex-shrink: 0;
    }
    .message {
        padding: 12px 18px;
        border-radius: 12px;
        max-width: 80%;
        line-height: 1.6;
        font-size: 0.95rem;
        width: fit-content;
        white-space: pre-wrap;
    }
    .user {
        align-self: flex-end;
        background-color: #2d2d2d;
        border-bottom-right-radius: 4px;
    }
    .ia {
        align-self: flex-start;
        background-color: #121212;
        border: 1px solid #333;
        border-bottom-left-radius: 4px;
    }
    .chat-area::-webkit-scrollbar {
        width: 8px;
    }
    .chat-area::-webkit-scrollbar-track {
        background: transparent;
    }
    .chat-area::-webkit-scrollbar-thumb {
        background-color: #444;
        border-radius: 4px;
    }
    .chat-area::-webkit-scrollbar-thumb:hover {
        background-color: #555;
    }
    .metrics-container {
        display: flex;
        gap: 10px;
        margin-top: 10px;
        margin-left: 15px;
        flex-wrap: wrap;
        font-family: 'Segoe UI', system-ui, sans-serif;
    }

    .metric-pill {
        display: flex;
        align-items: center;
        gap: 6px;
        background-color: #2b2b2b;
        padding: 4px 10px;
        border-radius: 6px;
        font-size: 0.85rem;
        border: 1px solid #3e3e3e;
    }

    .value {
        font-weight: 500;
        font-variant-numeric: tabular-nums;
    }

    .unit, .icon {
        color: #9aa0a6;
    }

    .icon {
        width: 14px;
        height: 14px;
        display: block;
    }

    .metric-pill:hover {
        background-color: #333;
        border-color: #555;
    }
</style>