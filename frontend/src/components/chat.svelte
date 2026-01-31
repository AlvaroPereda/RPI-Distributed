<script lang="ts">
    import type { Prompt } from "$lib/types";

    let { history }: { history: Prompt[] }= $props()

</script>

<div id="chat-container">
    <div class="chat-area">
        {#each history as {role, content, metrics}}
            <div class="message {role}">
                {content}
            </div>
        {/each}
    </div>
</div>

<style>
    #chat-container {
        display: flex;
        justify-content: center;
        padding: 20px;
        margin-top: 90px;
    }
    .chat-area {
        display: flex;
        flex-direction: column;
        width: 100%;
        max-width: 800px;
        gap: 15px;
    }
    .message {
        padding: 12px 18px;
        border-radius: 12px;
        color: white;
        max-width: 70%;
        line-height: 1.4;
    }
    .user {
        align-self: flex-end;
        background-color: #353535;
    }
    .assistant {
        align-self: flex-start;
        background-color: #565656;
    }
</style>