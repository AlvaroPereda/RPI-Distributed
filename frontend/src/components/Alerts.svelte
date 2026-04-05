<script lang="ts">
    import { notificationStore } from "$lib/notification.svelte";
    import Toast from "./Toast.svelte";
</script>

<div class="alerts-container">
    {#each notificationStore.visible as notification (notification.id)}
        <Toast {notification} />
    {/each}
</div>

<style>
    .alerts-container {
        position: fixed;
        bottom: 1.5rem;
        left: 1.5rem;
        z-index: 1000;
        display: flex;
        flex-direction: column;
        gap: 8px;
    }
</style>
