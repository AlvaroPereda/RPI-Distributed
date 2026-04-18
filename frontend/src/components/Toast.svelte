<script lang="ts">
    import { onMount } from "svelte";
    import { notificationStore } from "$lib/notification.svelte";
    import type { Notification } from "$lib/types";

    let { notification }: { notification: Notification } = $props()

    onMount(() => {
        if (notification.timeout === 0) return

        const timer = setTimeout(() => {
            notificationStore.remove(notification.id)
        }, notification.timeout)

        return () => clearTimeout(timer)
    })
</script>

<div class="toast {notification.type}" style="--duration: {notification.timeout}ms">
    <div class="toast-body">
        <span class="icon">
            {#if notification.type === 'success'}
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                    <polyline points="20 6 9 17 4 12"></polyline>
                </svg>
            {:else if notification.type === 'info'}
                <svg class="spinner" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round">
                    <circle cx="12" cy="12" r="9" stroke-dasharray="30 56"></circle>
                </svg>
            {:else}
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
                    <line x1="18" y1="6" x2="6" y2="18"></line>
                    <line x1="6" y1="6" x2="18" y2="18"></line>
                </svg>
            {/if}
        </span>
        <span class="message">{notification.message}</span>
        <button class="close" aria-label="Close notification" onclick={() => notificationStore.remove(notification.id)}>
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <line x1="18" y1="6" x2="6" y2="18"></line>
                <line x1="6" y1="6" x2="18" y2="18"></line>
            </svg>
        </button>
    </div>
    {#if notification.timeout > 0}
        <div class="progress-bar"></div>
    {/if}
</div>

<style>
    .toast {
        position: relative;
        min-width: 280px;
        max-width: 380px;
        background-color: var(--bg-surface-elevated);
        border: 1px solid var(--color-border);
        border-radius: var(--border-radius-md);
        overflow: hidden;
        animation: slide-in 0.2s ease;
    }

    .toast.success {
        border-left: 3px solid var(--color-accent-green);
    }

    .toast.error {
        border-left: 3px solid var(--color-accent-red);
    }

    .toast.info {
        border-left: 3px solid var(--color-accent-blue, #60a5fa);
    }

    .toast-body {
        display: flex;
        align-items: center;
        gap: 10px;
        padding: 12px 14px;
    }

    .icon {
        flex-shrink: 0;
        width: 16px;
        height: 16px;
        display: flex;
        align-items: center;
        justify-content: center;
    }

    .icon svg {
        width: 16px;
        height: 16px;
    }

    .toast.success .icon {
        color: var(--color-accent-green);
    }

    .toast.error .icon {
        color: var(--color-accent-red);
    }

    .toast.info .icon {
        color: var(--color-accent-blue, #60a5fa);
    }

    .spinner {
        animation: spin 1s linear infinite;
    }

    .message {
        flex: 1;
        font-family: var(--font-sans);
        font-size: 13px;
        color: var(--text-primary);
        line-height: 1.4;
    }

    .close {
        flex-shrink: 0;
        background: none;
        border: none;
        cursor: pointer;
        padding: 0;
        color: var(--text-muted);
        display: flex;
        align-items: center;
        transition: color 0.15s ease;
    }

    .close:hover {
        color: var(--text-primary);
    }

    .close svg {
        width: 14px;
        height: 14px;
    }

    .progress-bar {
        height: 2px;
        width: 100%;
        animation: shrink var(--duration) linear forwards;
    }

    .toast.success .progress-bar {
        background-color: var(--color-accent-green);
    }

    .toast.error .progress-bar {
        background-color: var(--color-accent-red);
    }

    .toast.info .progress-bar {
        background-color: var(--color-accent-blue, #60a5fa);
    }

    @keyframes shrink {
        from { width: 100%; }
        to { width: 0%; }
    }

    @keyframes spin {
        from { transform: rotate(0deg); }
        to { transform: rotate(360deg); }
    }

    @keyframes slide-in {
        from {
            opacity: 0;
            transform: translateX(-12px);
        }
        to {
            opacity: 1;
            transform: translateX(0);
        }
    }
</style>
