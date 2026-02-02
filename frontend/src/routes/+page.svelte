<script lang="ts">
    import type { Device } from "$lib/types";
    import ClusterCanvas from "../components/ClusterCanvas.svelte";
    import Settings from "../components/Settings.svelte";

    let device: Device[] = [
        {name: "rpi5", ip: "192.168.1.10"},
        {name: "rpi3", ip: "192.168.1.11"},
        {name: "rpi3b", ip: "192.168.1.12"}
    ]

</script>
<main>
    <ClusterCanvas cluster={device}/>
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