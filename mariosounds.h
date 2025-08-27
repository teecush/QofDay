<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>Inline Mario‑Style SFX (No External Files)</title>
  <style>
    body{font-family:ui-sans-serif,system-ui,Segoe UI,Roboto,Inter,Arial,sans-serif;margin:24px;}
    code{background:#f6f7fb;padding:.2em .4em;border-radius:6px}
    .muted{opacity:.7}
  </style>
</head>
<body>
  <h1>Inline Mario‑Style SFX (Web Audio synthesis)</h1>
  <p class="muted">Drop this <em>&lt;script&gt;</em> into your app; call <code>window.sfx.play(eventName)</code>, <code>startLoop(name)</code>, and <code>stopLoop(name)</code>. No .wav/.mp3 files needed.</p>

  <script>
  // Minimal, fileless chiptune SFX bank for a classroom name randomizer.
  // API:
  //   await sfx.init();                // call once on first user gesture
  //   sfx.play('startShuffle');
  //   sfx.play('tickFast');            // or 'tickSlow'
  //   sfx.startLoop('accelerateLoop'); // later: sfx.stopLoop('accelerateLoop')
  //   sfx.play('winnerLock'); sfx.play('winnerFanfare');
  //
  // Implemented events (sound-alikes, not Nintendo audio):
  // startShuffle, tickFast, tickSlow, accelerateLoop, decelerateBed,
  // preLock, winnerLock, winnerFanfare, confettiPop, removeFromPool,
  // undoReturn, reroll, error, muteOn, muteOff

  const sfx = (() => {
    const state = {
      ctx: null,
      master: null,
      loops: new Map(),
      muted: false,
      now: () => (state.ctx ? state.ctx.currentTime : 0)
    };

    function ensureCtx() {
      if (!state.ctx) {
        state.ctx = new (window.AudioContext || window.webkitAudioContext)({latencyHint: 'interactive'});
        state.master = state.ctx.createGain();
        state.master.gain.value = 0.9; // overall volume
        state.master.connect(state.ctx.destination);
      }
      if (state.ctx.state === 'suspended') state.ctx.resume();
    }

    async function init() { ensureCtx(); }

    // Helpers ---------------------------------------------------------------
    function env(g, t0, {attack=0.005, decay=0.05, sustain=0.6, release=0.06, peak=1.0, end=0.0001}={}) {
      const v = g.gain; v.cancelScheduledValues(t0);
      v.setValueAtTime(0.0001, t0);
      v.linearRampToValueAtTime(peak, t0 + attack);
      v.linearRampToValueAtTime(peak*sustain, t0 + attack + decay);
      return (releaseAt) => {
        v.cancelScheduledValues(releaseAt);
        v.setValueAtTime(v.value, releaseAt);
        v.linearRampToValueAtTime(end, releaseAt + release);
      };
    }

    function tone({type='square', freq=440, dur=0.12, vol=0.8, a=0.004, d=0.04, s=0.5, r=0.05, detune=0}) {
      ensureCtx();
      const t0 = state.now();
      const o = state.ctx.createOscillator();
      o.type = type; o.frequency.setValueAtTime(freq, t0); o.detune.value = detune;
      const g = state.ctx.createGain(); g.gain.value = 0;
      o.connect(g); g.connect(state.master);
      const release = env(g, t0, {attack:a, decay:d, sustain:s, release:r, peak:vol});
      o.start(t0); release(t0 + Math.max(0.01, dur)); o.stop(t0 + dur + r + 0.02);
      return { o, g };
    }

    function slide({from=440, to=880, dur=0.5, type='square', vol=0.8}) {
      ensureCtx(); const t0 = state.now();
      const o = state.ctx.createOscillator(); o.type = type; o.frequency.setValueAtTime(from, t0); o.frequency.linearRampToValueAtTime(to, t0+dur);
      const g = state.ctx.createGain(); g.gain.value = 0; o.connect(g); g.connect(state.master);
      const release = env(g, t0, {attack:0.01, decay:0.08, sustain:0.8, release:0.08, peak:vol});
      o.start(t0); release(t0 + dur); o.stop(t0 + dur + 0.1);
    }

    function noise({dur=0.15, vol=0.6, type='white', hp=0, lp=0, sweepLP=null}) {
      ensureCtx(); const t0 = state.now();
      const bufferSize = Math.floor(state.ctx.sampleRate * dur);
      const buffer = state.ctx.createBuffer(1, bufferSize, state.ctx.sampleRate);
      const data = buffer.getChannelData(0);
      for (let i=0;i<bufferSize;i++) data[i] = (Math.random()*2-1) * (type==='white'?1:type==='pink'?Math.random():1);
      const src = state.ctx.createBufferSource(); src.buffer = buffer; src.loop = false;
      let node = src;
      if (hp) { const hpF = state.ctx.createBiquadFilter(); hpF.type='highpass'; hpF.frequency.setValueAtTime(hp, t0); node.connect(hpF); node = hpF; }
      if (lp || sweepLP) { const lpF = state.ctx.createBiquadFilter(); lpF.type='lowpass'; lpF.frequency.setValueAtTime(lp||8000, t0); if (sweepLP) lpF.frequency.linearRampToValueAtTime(sweepLP, t0 + dur); node.connect(lpF); node = lpF; }
      const g = state.ctx.createGain(); g.gain.value = 0; node.connect(g); g.connect(state.master);
      const release = env(g, t0, {attack:0.002, decay:0.05, sustain:0.5, release:0.06, peak:vol});
      src.start(t0); release(t0 + dur); src.stop(t0 + dur + 0.1);
      return src;
    }

    // Loops -----------------------------------------------------------------
    function startLoop(name, fn, intervalMs=120) {
      stopLoop(name);
      const id = setInterval(fn, intervalMs);
      state.loops.set(name, {id, type:'interval'});
    }
    function stopLoop(name) {
      const rec = state.loops.get(name);
      if (!rec) return;
      if (rec.type === 'interval') clearInterval(rec.id);
      if (rec.type === 'source') try { rec.src.stop(); } catch {}
      state.loops.delete(name);
    }

    // Event implementations -------------------------------------------------
    function startShuffle() { // power-up arpeggio
      const root = 880; // A5
      const steps = [0, 4, 7, 12, 16];
      steps.forEach((st, i) => setTimeout(() => tone({type:'square', freq: root * Math.pow(2, st/12), dur:0.12, vol:0.75, a:0.002, d:0.03, s:0.4}), i*60));
    }

    function tickFast() { // coin blip
      tone({type:'square', freq: 1568, dur:0.05, vol:0.5, a:0.001, d:0.02, s:0.2});
    }
    function tickSlow() { // lower/longer blip
      tone({type:'square', freq: 1244, dur:0.08, vol:0.5, a:0.001, d:0.03, s:0.3});
    }

    function accelerateLoop_start() {
      stopLoop('accelerateLoop');
      // Fast arpeggio shimmer
      startLoop('accelerateLoop', () => {
        const base = 660;
        [0, 7, 12, 19].forEach((st, i) => setTimeout(() => tone({type:'triangle', freq: base*Math.pow(2, st/12), dur:0.07, vol:0.25}), i*18));
      }, 90);
    }

    function decelerateBed_start() {
      stopLoop('decelerateBed');
      // Soft roulette clicking via short filtered noise ticks
      startLoop('decelerateBed', () => {
        noise({dur:0.03, vol:0.25, hp:1200, lp:3000});
      }, 140);
    }

    function preLock() { // little drum fill: tom up + noise tap
      tone({type:'sine', freq:180, dur:0.08, vol:0.5});
      setTimeout(() => tone({type:'sine', freq:220, dur:0.08, vol:0.5}), 70);
      setTimeout(() => noise({dur:0.06, vol:0.35, hp:1000, lp:5000}), 140);
    }

    function winnerLock() { // flag-rise-like glide + bell overtone
      slide({from:392, to:784, dur:0.45, type:'square', vol:0.6});
      setTimeout(() => tone({type:'sine', freq:1568, dur:0.2, vol:0.45}), 180);
    }

    function winnerFanfare() { // short course-clear vibe
      const seq = [
        {t:0, f:784, d:0.12}, {t:120, f:988, d:0.12}, {t:240, f:1175, d:0.18},
        {t:480, f:1319, d:0.22}, {t:730, f:1568, d:0.28}
      ];
      seq.forEach(n => setTimeout(() => tone({type:'square', freq:n.f, dur:n.d, vol:0.6}), n.t));
      // add a noise sparkle tail
      setTimeout(() => noise({dur:0.18, vol:0.28, hp:3000, lp:8000}), 820);
    }

    function confettiPop() { noise({dur:0.08, vol:0.38, hp:1800, lp:8000}); }
    function removeFromPool() { tone({type:'square', freq:392, dur:0.12, vol:0.45, a:0.003, d:0.08, s:0.3}); }
    function undoReturn() { // vine-up arpeggio
      [0,5,9,12].forEach((st,i)=> setTimeout(()=> tone({type:'triangle', freq:330*Math.pow(2, st/12), dur:0.09, vol:0.45}), i*70));
    }
    function reroll() { // pipe whoosh: noise LP sweep
      noise({dur:0.28, vol:0.45, hp:200, lp:3000, sweepLP:900});
    }
    function error() { tone({type:'square', freq:196, dur:0.09, vol:0.55}); }
    function muteOn()  { tone({type:'square', freq:330, dur:0.06, vol:0.25}); }
    function muteOff() { tone({type:'square', freq:880, dur:0.06, vol:0.25}); }

    // Public routing --------------------------------------------------------
    const EVENT_MAP = {
      startShuffle: () => startShuffle(),
      tickFast: () => tickFast(),
      tickSlow: () => tickSlow(),
      accelerateLoop: {
        start: () => accelerateLoop_start(),
        stop: () => stopLoop('accelerateLoop')
      },
      decelerateBed: {
        start: () => decelerateBed_start(),
        stop: () => stopLoop('decelerateBed')
      },
      preLock: () => preLock(),
      winnerLock: () => winnerLock(),
      winnerFanfare: () => winnerFanfare(),
      confettiPop: () => confettiPop(),
      removeFromPool: () => removeFromPool(),
      undoReturn: () => undoReturn(),
      reroll: () => reroll(),
      error: () => error(),
      muteOn: () => muteOn(),
      muteOff: () => muteOff()
    };

    function play(evt) {
      const entry = EVENT_MAP[evt];
      if (!entry) return;
      if (typeof entry === 'function') entry();
      else if (entry.start) entry.start();
    }

    function startLoop(name) { const entry = EVENT_MAP[name]; if (entry && entry.start) entry.start(); }
    function stopLoop(name) { const entry = EVENT_MAP[name]; if (entry && entry.stop) entry.stop(); }
    function setMuted(on) { state.muted = on; state.master.gain.value = on ? 0 : 0.9; on ? muteOn() : muteOff(); }

    // Expose globally
    return { init, play, startLoop, stopLoop, setMuted };
  })();

  window.sfx = sfx;
  </script>

  <script>
    // Demo controls (safe to delete in integration)
    document.addEventListener('click', async () => { await sfx.init(); }, { once:true });
    console.log('sfx ready. Example timeline:');
    // Example sequence (run manually in devtools if you like):
    // await sfx.init(); sfx.play('startShuffle'); sfx.startLoop('accelerateLoop');
    // setTimeout(()=>{ sfx.startLoop('decelerateBed'); }, 1000);
    // setTimeout(()=>{ sfx.stopLoop('accelerateLoop'); sfx.play('preLock'); }, 1500);
    // setTimeout(()=>{ sfx.stopLoop('decelerateBed'); sfx.play('winnerLock'); sfx.play('winnerFanfare'); sfx.play('confettiPop'); setTimeout(()=>sfx.play('confettiPop'), 160);}, 1900);
  </script>
</body>
</html>