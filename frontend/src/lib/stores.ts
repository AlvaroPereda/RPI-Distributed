import { writable } from 'svelte/store';
import { browser } from '$app/environment';

const storedModel = browser ? window.localStorage.getItem('selected_model') : '';

export const model = writable(storedModel || '');

if (browser) {
    model.subscribe((value) => {
        if (value) window.localStorage.setItem('selected_model', value);
    });
}