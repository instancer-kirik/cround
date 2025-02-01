<script lang="ts">
  import { onMount } from 'svelte';
  import type { Clay_Vector2, Clay_Color } from '@clay/types';

  export let selectedGemstone: {
    id: string;
    name: string;
    image?: string;
    color: Clay_Color;
  } | null = null;

  let canvas: HTMLCanvasElement;
  let braceletRadius: number = 150;
  let beadRadius: number = 20;
  let numBeadSlots: number = 18;
  let beads: Array<{
    position: Clay_Vector2;
    color: Clay_Color;
    image?: string;
    id?: string;
    name?: string;
  }> = [];
  let ctx: CanvasRenderingContext2D | null = null;
  let hoveredBeadIndex: number = -1;

  onMount(() => {
    ctx = canvas.getContext('2d');
    if (!ctx) return;

    // Match canvas size to container
    const resizeCanvas = () => {
      const container = canvas.parentElement;
      if (!container) return;
      canvas.width = container.clientWidth;
      canvas.height = container.clientHeight;
      drawBracelet();
    };

    resizeCanvas();
    window.addEventListener('resize', resizeCanvas);

    // Initialize bead slots
    for (let i = 0; i < numBeadSlots; i++) {
      const angle = (i / numBeadSlots) * Math.PI * 2;
      beads.push({
        position: {
          x: Math.cos(angle) * braceletRadius,
          y: Math.sin(angle) * braceletRadius
        },
        color: { r: 0.5, g: 0.5, b: 0.5, a: 0.3 }
      });
    }

    return () => {
      window.removeEventListener('resize', resizeCanvas);
    };
  });

  function handleCanvasClick(event: MouseEvent) {
    if (!ctx || !selectedGemstone) return;

    const rect = canvas.getBoundingClientRect();
    const x = event.clientX - rect.left - canvas.width / 2;
    const y = event.clientY - rect.top - canvas.height / 2;

    const clickedIndex = beads.findIndex((bead) => {
      const dx = bead.position.x - x;
      const dy = bead.position.y - y;
      return Math.sqrt(dx * dx + dy * dy) <= beadRadius;
    });

    if (clickedIndex !== -1) {
      beads[clickedIndex] = {
        ...beads[clickedIndex],
        color: selectedGemstone.color,
        image: selectedGemstone.image,
        id: selectedGemstone.id,
        name: selectedGemstone.name
      };
      drawBracelet();
    }
  }

  function handleCanvasMouseMove(event: MouseEvent) {
    if (!ctx) return;

    const rect = canvas.getBoundingClientRect();
    const x = event.clientX - rect.left - canvas.width / 2;
    const y = event.clientY - rect.top - canvas.height / 2;

    const newHoveredIndex = beads.findIndex((bead) => {
      const dx = bead.position.x - x;
      const dy = bead.position.y - y;
      return Math.sqrt(dx * dx + dy * dy) <= beadRadius;
    });

    if (newHoveredIndex !== hoveredBeadIndex) {
      hoveredBeadIndex = newHoveredIndex;
      drawBracelet();
    }
  }

  function handleCanvasMouseLeave() {
    hoveredBeadIndex = -1;
    drawBracelet();
  }

  function drawBracelet() {
    if (!ctx) return;
    
    // Clear canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Center the bracelet
    ctx.save();
    ctx.translate(canvas.width / 2, canvas.height / 2);
    
    // Draw the bracelet circle
    ctx.beginPath();
    ctx.arc(0, 0, braceletRadius, 0, Math.PI * 2);
    ctx.strokeStyle = 'rgba(200, 200, 255, 0.5)';
    ctx.lineWidth = 2;
    ctx.stroke();
    
    // Draw bead slots
    beads.forEach((bead, index) => {
      ctx.beginPath();
      ctx.arc(bead.position.x, bead.position.y, beadRadius, 0, Math.PI * 2);

      if (index === hoveredBeadIndex) {
        ctx.shadowColor = 'rgba(255, 255, 255, 0.5)';
        ctx.shadowBlur = 10;
      } else {
        ctx.shadowColor = 'transparent';
        ctx.shadowBlur = 0;
      }

      if (bead.image) {
        // Load and draw bead image
        const img = new Image();
        img.src = bead.image;
        img.onload = () => {
          ctx?.save();
          ctx?.beginPath();
          ctx?.arc(bead.position.x, bead.position.y, beadRadius, 0, Math.PI * 2);
          ctx?.clip();
          ctx?.drawImage(
            img,
            bead.position.x - beadRadius,
            bead.position.y - beadRadius,
            beadRadius * 2,
            beadRadius * 2
          );
          ctx?.restore();
        };
      } else {
        // Draw colored circle
        ctx.fillStyle = `rgba(${bead.color.r * 255}, ${bead.color.g * 255}, ${bead.color.b * 255}, ${bead.color.a})`;
        ctx.fill();
      }

      ctx.strokeStyle = index === hoveredBeadIndex ? 'rgba(255, 255, 255, 0.8)' : 'rgba(200, 200, 255, 0.5)';
      ctx.lineWidth = index === hoveredBeadIndex ? 3 : 2;
      ctx.stroke();
    });
    
    ctx.restore();
  }

  $: if (selectedGemstone) {
    canvas?.style.setProperty('cursor', 'pointer');
  } else {
    canvas?.style.setProperty('cursor', 'default');
  }
</script>

<div class="bracelet-maker">
  <canvas
    bind:this={canvas}
    on:click={handleCanvasClick}
    on:mousemove={handleCanvasMouseMove}
    on:mouseleave={handleCanvasMouseLeave}
  />
</div>

<style>
  .bracelet-maker {
    width: 100%;
    height: 100%;
    background: rgba(20, 20, 25, 0.95);
    display: flex;
    align-items: center;
    justify-content: center;
  }

  canvas {
    width: 100%;
    height: 100%;
  }
</style> 