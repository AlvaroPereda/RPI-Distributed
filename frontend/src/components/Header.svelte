<script lang="ts">
    import { model } from "$lib/stores";
    import { goto } from "$app/navigation";

    const goBack = () => {
        goto("/")
    }
</script>

<header class="header">
    <button onclick={goBack}>Back to Config</button>
    <h1>{$model}</h1>
</header>

<style>
    .header {
        display: flex;
        align-items: center;
        justify-content: center;
        background-color: rgb(62, 62, 62);
        border-bottom: 3px solid #444;
        position: fixed;
        top: 0;
        left: 0;
        right: 0;
        width: 100%;
        z-index: 100;

        button {
            cursor: pointer;
            position: absolute;
            left: 50px;
            background: transparent;
            color: white;
            border: 3px solid #888;
            border-radius: 5px;
            font-size: large;
        }
    }
</style>