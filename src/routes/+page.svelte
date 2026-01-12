<script lang="ts">
    import type { Prompt } from "$lib/types";

    import Chat from "./chat.svelte";
    import Header from "./header.svelte";
    import Input from "./input.svelte";

    const history: Prompt[] = $state([])

    function handleNewPrompt(prompt: string) {
        history.push({role: "user", content: prompt})
    }

</script>
<main>
    <Header />
    <Chat history={history}/>
    <Input onPrompt={handleNewPrompt}/>
</main>

<style>
    main {
        max-width: 800px; margin: 0 auto; padding: 20px; font-family: sans-serif;
    }
</style>