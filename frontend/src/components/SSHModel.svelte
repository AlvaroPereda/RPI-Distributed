<script lang="ts">
    import { fade, scale } from 'svelte/transition';
    import type { Device } from "$lib/types";
    import { deviceState } from '$lib/deviceState.svelte';
    import { notificationStore } from '$lib/notification.svelte';

    let isOpen = $state(false)
    let device: Device = $state({ user: "", ip: "", password: "" })

    const handleKeydown = (e: KeyboardEvent) => {
        if (e.key === "Escape" && isOpen) isOpen = false
    }

    const connectDevice = async() => {
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

            if(!response.ok) {
                const errorData = await response.json()
                throw new Error(errorData.error)
            }

            deviceState.setConnection({...device})
            device = {user: "", ip: "", password: ""}

            isOpen = false;
        } catch (e) {
            notificationStore.add("error", e instanceof Error ? e.message : "An unexpected error occurred on the server")
        }
    }
</script>

<svelte:window onkeydown={handleKeydown}/>

<button class="trigger-btn" onclick={() => isOpen = true}>
    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" width="13" height="13">
        <line x1="12" y1="5" x2="12" y2="19"></line>
        <line x1="5" y1="12" x2="19" y2="12"></line>
    </svg>
    Add a device
</button>

{#if isOpen}
    <button
        class="backdrop"
        transition:fade={{duration: 150}}
        onclick={() => isOpen = false}
        type="button"
        aria-label="Cerrar modal"
    ></button>

    <div class="modal-card" transition:scale={{start: 0.96, duration: 180}}>
        <div class="modal-header">
            <div class="modal-title-row">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round" width="16" height="16" class="modal-icon">
                    <rect width="16" height="16" x="4" y="4" rx="2"/>
                    <rect width="6" height="6" x="9" y="9" rx="1"/>
                    <path d="M15 2v2"/><path d="M15 20v2"/><path d="M2 15h2"/><path d="M2 9h2"/><path d="M20 15h2"/><path d="M20 9h2"/><path d="M9 2v2"/><path d="M9 20v2"/>
                </svg>
                <h3>Add new device</h3>
            </div>
            <button class="close-btn" onclick={() => isOpen = false} aria-label="Close">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" width="16" height="16">
                    <line x1="18" y1="6" x2="6" y2="18"></line>
                    <line x1="6" y1="6" x2="18" y2="18"></line>
                </svg>
            </button>
        </div>

        <form>
            <div class="input-group">
                <label for="name">User</label>
                <input type="text" id="name" bind:value={device.user} placeholder="admin" autocomplete="off">
            </div>

            <div class="input-group">
                <label for="ip">IP address</label>
                <input type="text" id="ip" placeholder="192.168.1.1" bind:value={device.ip} autocomplete="off">
            </div>

            <div class="input-group">
                <label for="password">Password</label>
                <input type="password" id="password" bind:value={device.password} placeholder="••••••••">
            </div>

            <button type="button" class="action-btn" onclick={connectDevice}>
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" width="13" height="13">
                    <path d="M5 12h14M12 5l7 7-7 7"/>
                </svg>
                Connect
            </button>
        </form>
    </div>
{/if}

<style>
    .trigger-btn {
        display: flex;
        align-items: center;
        gap: 7px;
        background-color: transparent;
        color: var(--color-accent-green);
        font-family: var(--font-sans);
        font-size: 13px;
        font-weight: 500;
        padding: 9px 14px;
        border: 1px solid rgba(74, 222, 128, 0.3);
        border-radius: var(--border-radius-md);
        cursor: pointer;
        transition: background-color 0.2s ease, border-color 0.2s ease, box-shadow 0.2s ease;
        width: 100%;
        justify-content: center;
    }

    .trigger-btn:hover {
        background-color: rgba(74, 222, 128, 0.06);
        border-color: rgba(74, 222, 128, 0.5);
        box-shadow: 0 0 12px rgba(74, 222, 128, 0.1);
    }

    /* Backdrop */
    .backdrop {
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(0, 0, 0, 0.75);
        backdrop-filter: blur(3px);
        -webkit-backdrop-filter: blur(3px);
        z-index: 50;
        border: none;
        cursor: default;
    }

    /* Modal */
    .modal-card {
        position: fixed;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        z-index: 51;
        width: 90%;
        max-width: 400px;
        background-color: var(--bg-surface);
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-lg);
        box-shadow: 0 24px 48px rgba(0, 0, 0, 0.7), 0 0 0 1px rgba(74, 222, 128, 0.05);
        overflow: hidden;
    }

    .modal-header {
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding: 20px 20px 16px;
        border-bottom: 1px solid var(--color-border);
    }

    .modal-title-row {
        display: flex;
        align-items: center;
        gap: 10px;
    }

    .modal-icon {
        color: var(--color-accent-green);
        flex-shrink: 0;
    }

    .modal-header h3 {
        margin: 0;
        font-family: var(--font-sans);
        font-size: 15px;
        font-weight: 600;
        color: var(--text-primary);
    }

    .close-btn {
        background: transparent;
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-md);
        color: var(--text-muted);
        width: 30px;
        height: 30px;
        display: flex;
        align-items: center;
        justify-content: center;
        cursor: pointer;
        transition: color 0.2s ease, border-color 0.2s ease;
        padding: 0;
        flex-shrink: 0;
    }

    .close-btn:hover {
        color: var(--text-primary);
        border-color: var(--text-muted);
    }

    form {
        display: flex;
        flex-direction: column;
        gap: 14px;
        padding: 20px;
    }

    .input-group {
        display: flex;
        flex-direction: column;
        gap: 6px;
    }

    label {
        font-family: var(--font-sans);
        font-size: 11px;
        font-weight: 600;
        color: var(--text-muted);
        text-transform: uppercase;
        letter-spacing: 0.07em;
    }

    input {
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        color: var(--text-primary);
        font-family: var(--font-sans);
        font-size: 13px;
        padding: 10px 12px;
        border-radius: var(--border-radius-md);
        outline: none;
        transition: border-color 0.2s ease;
        width: 100%;
        box-sizing: border-box;
    }

    input:focus {
        border-color: var(--text-muted);
    }

    input::placeholder {
        color: var(--text-muted);
        opacity: 0.6;
    }

    .action-btn {
        display: flex;
        align-items: center;
        justify-content: center;
        gap: 8px;
        margin-top: 4px;
        background-color: var(--color-accent-green);
        color: #000;
        font-family: var(--font-sans);
        font-size: 13px;
        font-weight: 600;
        padding: 11px;
        border: none;
        border-radius: var(--border-radius-md);
        cursor: pointer;
        box-shadow: var(--shadow-glow);
        transition: background-color 0.2s ease, box-shadow 0.2s ease, transform 0.15s ease;
        width: 100%;
    }

    .action-btn:hover {
        background-color: #6ee8a0;
        box-shadow: 0 0 22px rgba(74, 222, 128, 0.5);
        transform: translateY(-1px);
    }

    .action-btn:active {
        transform: translateY(0);
    }

    .error {
        display: flex;
        align-items: center;
        gap: 7px;
        color: var(--color-accent-red);
        font-family: var(--font-sans);
        font-size: 12px;
        margin: 0;
        background-color: rgba(248, 113, 113, 0.08);
        border: 1px solid rgba(248, 113, 113, 0.2);
        border-radius: var(--border-radius-md);
        padding: 9px 12px;
    }
</style>
