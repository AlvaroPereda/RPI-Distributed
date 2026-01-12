<script lang="ts">
    import type { Prompt } from "$lib/types";

    type ChatProps = {
        history: Prompt[]
    }

    let { history }: ChatProps= $props()

</script>

<div class="chat-area">
    {#each history as {role, content}}
        <div class="message {role}">
            {content}
        </div>
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
</style>