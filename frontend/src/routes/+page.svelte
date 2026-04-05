<script lang="ts">
    import ClusterCanvas from "../components/ClusterCanvas.svelte";
    import Settings from "../components/Settings.svelte";
</script>

<main class="config-page">
    <ClusterCanvas />
    <Settings />
</main>

<style>
    .config-page {
        display: flex;
        position: relative;
        align-items: center;
        justify-content: center;
        width: 100vw;
        height: 100vh;
        overflow: hidden;
        padding-right: 300px;
        box-sizing: border-box;
        background-color: var(--bg-deep);
        background-image:
            radial-gradient(circle, rgba(74, 222, 128, 0.03) 1px, transparent 1px);
        background-size: 32px 32px;
    }
</style>
