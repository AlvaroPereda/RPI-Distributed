<script lang="ts">
    import { fade, scale } from 'svelte/transition';
    import type { Device } from "$lib/types";
    import { deviceState } from '$lib/deviceState.svelte';

    let isOpen = $state(false)
    let device: Device = $state({ user: "", ip: "", password: "" })
    let errorMessage = $state("")

    const handleKeydown = (e: KeyboardEvent) => {
        if (e.key === "Escape" && isOpen) isOpen = false
    }

    const connectDevice = async() => {
        errorMessage = ""
        try {
            const response = await fetch("/connect", {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    user: device.user,
                    ip: device.ip,
                    password: device.password
                })
            })

            if(!response.ok) throw new Error("Error en la respuesta del servidor.")

            deviceState.setConnection({...device})
            device = {user: "", ip: "", password: ""}

            isOpen = false;
        } catch (e) {
            errorMessage = e instanceof Error ? e.message : "Error desconocido"
        }
    }
</script>

<svelte:window onkeydown={handleKeydown}/>

<button class="trigger-btn" onclick={() => isOpen = true}>
    Add a device
</button>

{#if isOpen}
    <button 
        class="backdrop" 
        transition:fade 
        onclick={() => isOpen = false}
        type="button"
        aria-label="Cerrar modal"
    ></button>

    <div class="modal-card" transition:scale={{start: 0.95, duration: 200}}>
        <div class="header">
            <h3>Add new device</h3>
            <button class="close-btn" onclick={() => isOpen = false}>&times;</button>
        </div>

        <form>
            <div class="input-group">
                <label for="name">User</label>
                <input type="text" id="name" bind:value={device.user} placeholder="admin">
            </div>

            <div class="input-group">
                <label for="ip">IP address</label>
                <input type="text" id="ip" placeholder="192.168.1.1" bind:value={device.ip}>
            </div>

            <div class="input-group">
                <label for="password">Password</label>
                <input type="password" id="password" bind:value={device.password} placeholder="••••••">
            </div>

            {#if errorMessage}
                <p class="error">{errorMessage}</p>
            {/if}

            <button type="button" class="action-btn" onclick={connectDevice}>Connect</button>
        </form>
    </div>
{/if}

<style>
    .trigger-btn {
        background-color: rgb(62, 62, 62);
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 6px;
        cursor: pointer;
        font-weight: 500;
    }

    .backdrop {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(0, 0, 0, 0.7);
        z-index: 50;
        backdrop-filter: blur(2px);
    }

    .modal-card {
        position: fixed;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        z-index: 51;
        width: 90%;
        max-width: 400px;
        padding: 2rem;
        background-color: #1a1a1a;
        border: 1px solid #333;
        border-radius: 12px;
        box-shadow: 0 20px 25px -5px rgba(0, 0, 0, 0.5), 0 10px 10px -5px rgba(0, 0, 0, 0.4);
        color: #e5e5e5;
    }

    .header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 1.5rem;
    }

    .header h3 {
        margin: 0;
        font-size: 1.25rem;
        color: #fff;
    }

    .close-btn {
        background: transparent;
        border: none;
        color: #888;
        font-size: 1.5rem;
        cursor: pointer;
        transition: color 0.2s;
    }

    .close-btn:hover {
        color: #fff;
    }

    form {
        display: flex;
        flex-direction: column;
        gap: 1rem;
    }

    .input-group {
        display: flex;
        flex-direction: column;
        gap: 0.5rem;
    }

    label {
        font-size: 0.875rem;
        color: #a3a3a3;
        font-weight: 500;
    }

    input {
        background-color: #262626;
        border: 1px solid #404040;
        color: white;
        padding: 0.75rem;
        border-radius: 6px;
        outline: none;
        transition: border-color 0.2s;
    }

    input:focus {
        border-color: rgb(62, 62, 62);
    }

    input::placeholder {
        color: #525252;
    }

    .action-btn {
        margin-top: 1rem;
        background-color: rgb(62, 62, 62);
        color: white;
        padding: 0.75rem;
        border: none;
        border-radius: 6px;
        font-weight: 600;
        cursor: pointer;
        transition: background-color 0.2s;
    }

    .action-btn:hover {
        background-color: rgb(95, 95, 95);
    }

    .error {
        color: #ef4444;
        font-size: 0.875rem;
        margin: 0;
        text-align: center;
    }
</style>