<script lang="ts">
  import { createEventDispatcher } from 'svelte';
  import type { Clay_Color } from '@clay/types';

  export let gemstones: Array<{
    id: string;
    name: string;
    image?: string;
    color: Clay_Color;
  }> = [
    { id: 'ruby', name: 'Ruby', color: { r: 0.8, g: 0.1, b: 0.1, a: 1.0 } },
    { id: 'sapphire', name: 'Sapphire', color: { r: 0.1, g: 0.1, b: 0.8, a: 1.0 } },
    { id: 'emerald', name: 'Emerald', color: { r: 0.1, g: 0.8, b: 0.1, a: 1.0 } },
    { id: 'amethyst', name: 'Amethyst', color: { r: 0.6, g: 0.2, b: 0.8, a: 1.0 } },
    { id: 'topaz', name: 'Topaz', color: { r: 0.8, g: 0.6, b: 0.1, a: 1.0 } },
    { id: 'diamond', name: 'Diamond', color: { r: 0.9, g: 0.9, b: 0.9, a: 1.0 } },
  ];

  const dispatch = createEventDispatcher();

  function handleDragStart(event: DragEvent, gemstone: typeof gemstones[0]) {
    if (!event.dataTransfer) return;
    event.dataTransfer.setData('application/json', JSON.stringify(gemstone));
    event.dataTransfer.effectAllowed = 'copy';
  }

  function handleGemstoneClick(gemstone: typeof gemstones[0]) {
    dispatch('select', gemstone);
  }
</script>

<div class="bead-palette">
  <h2>Gemstones</h2>
  <div class="gemstone-grid">
    {#each gemstones as gemstone}
      <div
        class="gemstone"
        draggable="true"
        on:dragstart={(e) => handleDragStart(e, gemstone)}
        on:click={() => handleGemstoneClick(gemstone)}
      >
        {#if gemstone.image}
          <img src={gemstone.image} alt={gemstone.name} />
        {:else}
          <div
            class="gemstone-color"
            style="background-color: rgba({gemstone.color.r * 255}, {gemstone.color.g * 255}, {gemstone.color.b * 255}, {gemstone.color.a})"
          />
        {/if}
        <span class="gemstone-name">{gemstone.name}</span>
      </div>
    {/each}
  </div>
</div>

<style>
  .bead-palette {
    background: rgba(30, 30, 35, 0.95);
    padding: 1rem;
    border-radius: 8px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
  }

  h2 {
    color: #fff;
    margin: 0 0 1rem 0;
    font-size: 1.2rem;
  }

  .gemstone-grid {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 1rem;
  }

  .gemstone {
    display: flex;
    flex-direction: column;
    align-items: center;
    cursor: pointer;
    padding: 0.5rem;
    border-radius: 4px;
    background: rgba(40, 40, 45, 0.8);
    transition: transform 0.2s, background 0.2s;
  }

  .gemstone:hover {
    transform: translateY(-2px);
    background: rgba(50, 50, 55, 0.8);
  }

  .gemstone-color {
    width: 40px;
    height: 40px;
    border-radius: 50%;
    margin-bottom: 0.5rem;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
  }

  .gemstone img {
    width: 40px;
    height: 40px;
    border-radius: 50%;
    margin-bottom: 0.5rem;
    object-fit: cover;
  }

  .gemstone-name {
    color: #fff;
    font-size: 0.9rem;
    text-align: center;
  }
</style> 