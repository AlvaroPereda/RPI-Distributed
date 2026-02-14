<script lang="ts">
    import { model } from "$lib/stores";
    import { goto } from "$app/navigation";
    import SSHModel from "./SSHModel.svelte";

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

<nav>
    <h1>CLUSTER SETTINGS</h1>

    <label for="model-select">Select Model:</label>
    <select id="model-select" bind:value={$model}>
        <option value="" disabled>Models</option>
        <option value="ggml-org/gemma-3-1b-it-GGUF">ggml-org/gemma-3-1b-it-GGUF</option>
        <option value="bartowski/Llama-3.2-1B-Instruct-GGUF">bartowski/Llama-3.2-1B-Instruct-GGUF</option>
    </select>

    <button onclick={startChat}>START CHAT</button>
    <SSHModel />
</nav>

<style>
    nav {
        flex-grow: 1;
        position: absolute;
        top: 0;
        right: 0;
        width: 300px;
        height: 100vh;
        padding: 2rem;
        background-color: rgb(62, 62, 62);
    }
</style>