<script lang="ts">
    import BraceletMaker from './components/BraceletMaker.svelte';
    import BeadPalette from '../../../cround/BeadPalette.svelte';
    import type { Clay_Color } from '@clay/types';
  
    let selectedGemstone: {
      id: string;
      name: string;
      image?: string;
      color: Clay_Color;
    } | null = null;
  
    function handleGemstoneSelect(event: CustomEvent) {
      selectedGemstone = event.detail;
    }
  </script>
  
  <main>
    <div class="app-container">
      <div class="bracelet-area">
        <BraceletMaker {selectedGemstone} />
      </div>
      <div class="palette-area">
        <BeadPalette on:select={handleGemstoneSelect} />
      </div>
    </div>
  </main>
  
  <style>
    main {
      width: 100vw;
      height: 100vh;
      background: linear-gradient(135deg, #1a1a1f 0%, #2a2a35 100%);
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 2rem;
    }
  
    .app-container {
      width: 100%;
      max-width: 1200px;
      height: 100%;
      display: grid;
      grid-template-columns: 1fr 300px;
      gap: 2rem;
    }
  
    .bracelet-area {
      background: rgba(25, 25, 30, 0.8);
      border-radius: 12px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
      overflow: hidden;
    }
  
    .palette-area {
      display: flex;
      flex-direction: column;
      gap: 1rem;
    }
  
    @media (max-width: 768px) {
      .app-container {
        grid-template-columns: 1fr;
        grid-template-rows: 1fr auto;
      }
    }
  </style> 