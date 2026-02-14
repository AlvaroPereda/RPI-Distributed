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

<div class="container-input">
    <div>
        <textarea 
            bind:value={prompt}
            onkeydown={handleKeydown}
            placeholder="Type a message"
        ></textarea>
        <button type="button" aria-label="Attachments" onclick={sendPrompt}>
            <svg viewBox="0 0 24 24" fill="currentColor">
                <path d="M2.01 21L23 12 2.01 3 2 10l15 2-15 2z"></path>
            </svg>
        </button>
    </div>
</div>

<style>
    .container-input {
        display: flex;
        align-items: center;
        justify-content: center;
        position: fixed;
        bottom: 0;
        left: 0;
        right: 0;
        background-color: rgb(44, 44, 44);
        padding: 10px;
        box-shadow: 0 -2px 5px rgba(140, 140, 140, 0.1);


        textarea {
            background-color: transparent;
            color: white;
            border: 1px solid rgba(140, 140, 140, 0.1);
            resize: none;
            border-radius: 5px;
            width: 700px;
            height: 100px;
            font-size: 16px;
        }

        button {
            background-color: rgb(62, 62, 62);
            width: 40px;
            height: 40px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin-left: 10px;
            margin-bottom: 5px;
        }

        button:hover {
            background-color: rgb(80, 80, 80);
        }

        button svg {
            width: 20px;
            height: 20px;
        }
    }
</style>