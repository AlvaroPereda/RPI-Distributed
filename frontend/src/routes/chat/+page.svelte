<script lang="ts">
    import type { Prompt } from "$lib/types";
    import Chat from "../../components/chat.svelte";
    import Header from "../../components/header.svelte";
    import Input from "../../components/input.svelte";


    let history:Prompt = {
        role: "user",
        content: "buenas, como estás"
    }
    let history2:Prompt = {
        role: "assistant",
        content: "Buenas soy un asistente de IA"
    }
</script>
<div>
    <Header model="ggml-org/gemma-3-1b-it-GGUF"/>
    <Chat history={[history, history2]}/>
    <Input />
</div>