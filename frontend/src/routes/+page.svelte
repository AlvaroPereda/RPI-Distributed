<script lang="ts">
    import ClusterCanvas from "../components/ClusterCanvas.svelte";
    import Settings from "../components/Settings.svelte";

</script>
<main>
    <ClusterCanvas />
    <Settings />
</main>

<style>
    main {
        display: flex;
        position: relative;
        align-items: center;
        justify-content: center;
        width: 100vw;
        height: 100vh;
        overflow: hidden;
        padding-right: 300px; 
        box-sizing: border-box;
    }
</style>