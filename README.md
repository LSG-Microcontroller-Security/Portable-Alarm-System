# Portable Alarm System

**Italiano:** Apri la sezione in italiano per consultare la guida d'uso.  
**English:** Expand the English section to read the user guide.

<details>
<summary><strong>Italiano — Guida all'utilizzo</strong></summary>

Portable Alarm System è un dispositivo compatto e modulare di allarme e monitoraggio, progettato per proteggere oggetti, ambienti, veicoli e altri spazi di dimensioni contenute. Rileva le condizioni di allarme e può chiamare un numero telefonico configurato tramite il modulo GSM. Il Bluetooth consente la configurazione locale e, nelle modalità previste, il rilevamento di uno smartphone autorizzato.

Il firmware è progettato per un microcontrollore compatibile con AVR/Arduino e può utilizzare sensori e accessori esterni.

> **Stato della documentazione:** questa guida descrive il firmware esaminato sulla branch `develop` il 23 ottobre 2026. Alcuni comportamenti dipendono dall'hardware installato, dal telefono, dal modulo Bluetooth e dalla versione del firmware. Prima di affidarsi al sistema di allarme, verificarne il comportamento sul dispositivo reale.

## Avvio rapido

1. Configurare i numeri telefonici autorizzati e, se si utilizza una modalità Find, l'indirizzo e il nome Bluetooth del telefono selezionato. Queste impostazioni si trovano nel menu Bluetooth **Configuration**.
2. Scegliere una modalità di funzionamento: movimento, contatto esterno, PIR oppure una delle due modalità di rilevamento del telefono (Find).
3. Verificare la chiamata GSM, il sensore selezionato e la ricerca Bluetooth nelle effettive condizioni di alimentazione e utilizzo del telefono.
4. Tenere presente che il firmware prevede un timer di inserimento automatico dell'allarme di circa **cinque minuti**. L'accesso ad alcune funzioni del menu Bluetooth azzera e riavvia questo timer. Il comando `Alarm OFF` non garantisce quindi che l'allarme rimanga disattivato indefinitamente.

La schermata principale del menu Bluetooth mostra **Alarm ON/OFF**, la versione del firmware, la temperatura, un'indicazione approssimativa del livello della batteria e `WhatzUp` (codice dell'ultimo evento registrato). Dalla schermata principale si possono aprire i menu **Configuration** e **Security** oppure cambiare lo stato dell'allarme.

## Modalità operative

| Modalità | Funzionamento | Comando SMS |
| --- | --- | --- |
| Movimento / inclinazione | Rileva movimento o inclinazione tramite il sensore interno e chiama il numero selezionato quando l'allarme è inserito. Non richiede il rilevamento Bluetooth del telefono. | `Md` |
| Contatto esterno normalmente aperto | Utilizza il contatto esterno come sorgente dell'allarme. | `Eo` |
| Contatto esterno normalmente chiuso | Utilizza il contatto esterno come sorgente dell'allarme. | `Ec` |
| PIR | Utilizza il sensore PIR esterno come sorgente dell'allarme; può essere attivato anche un buzzer opzionale. | `Wc` |
| **Find Mode 1** (esclusione per telefono presente) | Cerca il telefono autorizzato. Quando lo rileva, inibisce l'allarme di movimento interno (e le chiamate PIR, se il PIR è stato abilitato separatamente). L'attivazione di questa modalità via SMS azzera le altre modalità sensore, compreso il PIR. La logica del contatto esterno è separata. | `N1`–`N9` |
| **Find Mode 2** (allarme per telefono assente) | Controlla la presenza del telefono autorizzato; se non viene rilevato, può avviare una chiamata GSM. In questa modalità il ciclo principale del firmware esclude i normali allarmi dei sensori. | `F1`–`F9` |

Per i comandi `N1`–`N9` e `F1`–`F9`, la cifra indica **il numero di tentativi di rilevamento del telefono per ciclo di ricerca** (da 1 a 9). Entrambe le modalità utilizzano lo stesso percorso di rilevamento Bluetooth. Il firmware può inoltre avviare una chiamata quando si attivano modalità che richiamano la routine di inserimento dell'allarme, comprese `F1`–`F9`.

Nel codice sono previste impostazioni per **due numeri telefonici** e **due coppie di identificativi Bluetooth**. L'opzione `N.Phone` seleziona il telefono attualmente utilizzato (1 oppure 2); la logica di ricerca controlla il relativo indirizzo e nome Bluetooth, senza cercare contemporaneamente entrambi i telefoni.

### Attenzione: hotspot dello smartphone e affidabilità della ricerca Bluetooth

> **Possibili interferenze su alcuni telefoni:** l'attivazione dell'**hotspot Wi-Fi/tethering** può rendere intermittente la ricerca Bluetooth o impedire al dispositivo di rilevare con continuità lo smartphone autorizzato. Il problema può interessare **sia Find Mode 1 (`N1`–`N9`) sia Find Mode 2 (`F1`–`F9`)**, poiché entrambe dipendono dalla rilevazione del telefono. L'osservazione riguarda specificamente la **funzione hotspot**, non necessariamente la connessione dati mobile o la rete cellulare.
>
> Se la rilevazione non è affidabile, per prima cosa **disattivare l'hotspot del telefono** e ripetere le prove mantenendo invariate le altre condizioni. Confrontare più ricerche con hotspot attivo e disattivato; verificare inoltre lo stato del Bluetooth, il nome e l'indirizzo configurati e la stabilità dell'alimentazione del modulo HC-05. La causa precisa e i modelli di telefono interessati **non sono ancora stati accertati definitivamente**: si tratta quindi di una precauzione diagnostica, non di un'incompatibilità generale dimostrata.

In **Find Mode 1**, una mancata rilevazione può impedire l'inibizione di un allarme movimento altrimenti valido (oppure di un allarme PIR abilitato separatamente). In **Find Mode 2**, una mancata rilevazione può essere interpretata come assenza del telefono e provocare una chiamata. Una singola ricerca riuscita non è sufficiente a dimostrare l'affidabilità del sistema.

## Controllo tramite SMS

Il firmware accetta comandi SMS da uno dei due numeri telefonici autorizzati configurati. I comandi sono **sensibili alle maiuscole e minuscole** e sono lunghi **esattamente due caratteri**; in `N1`–`N9` e `F1`–`F9` il secondo carattere è una cifra. I comandi impostano **lo stato corrente durante l'esecuzione**: non si deve presumere che tutte le modifiche rimangano memorizzate dopo uno spegnimento.

| Comando | Operazione |
| --- | --- |
| `N1`–`N9` | Attiva Find Mode 1; cifra = tentativi per ciclo di ricerca |
| `F1`–`F9` | Attiva Find Mode 2; cifra = tentativi per ciclo di ricerca |
| `Md` | Attiva l'allarme movimento/inclinazione senza ricerca Bluetooth |
| `Eo` | Attiva l'allarme con contatto esterno normalmente aperto |
| `Ec` | Attiva l'allarme con contatto esterno normalmente chiuso |
| `Ex` | Disabilita l'allarme del contatto esterno |
| `Wc` | Attiva l'allarme PIR senza ricerca Bluetooth |
| `Bz` | Abilita il buzzer opzionale (utilizzato nella gestione dell'allarme PIR nel firmware attuale) |
| `P1`, `P2` | Seleziona il numero principale/alternativo e avvia una chiamata |
| `Ck` | Effettua una chiamata di verifica al numero selezionato |
| `Rc` | Abilita la risposta automatica alle chiamate in ingresso |
| `Rs` | Disabilita la risposta automatica e avvia una chiamata |
| `Dc` | Inibisce le chiamate di allarme e termina l'eventuale chiamata in corso |
| `Ab` | Accende il Bluetooth e apre una nuova finestra per la configurazione e il timer di inserimento |
| `Al` | Abilita i lampeggi del LED di alimentazione |

Questi sono i comandi implementati nella routine `listOfSmsCommands()` esaminata; **non** rappresentano tutte le possibili capacità hardware o funzionalità di versioni future del firmware. Alcuni comandi di attivazione avviano immediatamente una chiamata GSM: tenerne conto durante la configurazione.

## Menu Bluetooth

### Menu principale

Il menu principale mostra lo stato corrente dell'allarme e permette di selezionare **Alarm On / Alarm OFF**, **Configuration** e **Security**. Visualizza inoltre temperatura, livello della batteria e l'indicatore dell'ultimo evento (`WhatzUp`). Nel firmware esaminato le lettere utilizzate comprendono `M` (movimento interno), `E` (contatto esterno), `P` (PIR), `T` (temperatura elevata), `V` (tensione bassa) e `X` (stato iniziale).

### Menu Configuration

- **Phone / Ph.Altern.** — numeri telefonici autorizzati principale e alternativo.
- **N.Phone** — selezione del telefono 1 o 2 per le chiamate e la relativa voce di rilevamento Bluetooth.
- **TempMax / OffSetTemp** — soglia di allarme temperatura e offset di calibrazione del sensore.
- **PIR status** — abilita/disabilita l'ingresso PIR, quando la modalità selezionata lo consente.
- **Addr / Name / Addr2 / Name2** — identificativi Bluetooth configurati per i telefoni (visualizzati quando è attiva una modalità Find).
- **FindLoop / FindMode** — numero di tentativi di ricerca Bluetooth e modalità operativa (0 = Find disattivato, 1 = Find Mode 1, 2 = Find Mode 2).
- **Ext.Int / Buzz.** — abilitazione dell'interrupt esterno e del buzzer.

Molte modifiche effettuate tramite queste voci vengono salvate in EEPROM e ripristinate all'inizializzazione del firmware. Al contrario, la maggior parte dei comandi SMS modifica lo stato della sessione corrente.

### Menu Security

Il menu di sicurezza permette di modificare la **password** Bluetooth (dopo aver verificato quella esistente) e il **nome del dispositivo**. Dopo tali modifiche potrebbe essere necessario riconnettere il telefono.

## Sensori, monitoraggio e notifiche

Il firmware comprende un interrupt per movimento/inclinazione, un interrupt per il contatto esterno, un ingresso PIR, un buzzer opzionale, la lettura della temperatura interna del microcontrollore e della tensione di alimentazione. La soglia di tensione bassa è impostata a circa **3,25 V** nel firmware esaminato. Una temperatura elevata o una tensione bassa possono generare chiamate GSM indipendentemente dal normale percorso di allarme movimento.

**Le notifiche sono chiamate GSM**, non SMS in uscita di carattere generale. Gli SMS vengono utilizzati soprattutto per ricevere comandi da remoto. Verificare che la SIM e il modulo GSM riescano a effettuare chiamate e che la selezione del numero sia corretta.

## Configurazione iniziale e diagnostica

Il repository include uno sketch separato, `Portable-Alarm-Initialized`, per inizializzare la EEPROM ed eseguire la diagnostica Bluetooth. L'opzione `EEPROM_INITIALIZATION_ENABLED` è **disabilitata per impostazione predefinita** nel ramo esaminato. Se viene abilitata, la EEPROM viene cancellata e inizializzata nuovamente: salvare prima le impostazioni necessarie e utilizzare questa funzione consapevolmente.

All'avvio il firmware principale stampa `Firmware Git` e `Libraries Git`, consentendo di associare i test hardware alle revisioni del firmware e delle librerie. Le macro di debug seriale sono gestite separatamente dal menu Bluetooth e possono essere disabilitate nella normale compilazione.

## Risoluzione dei problemi

**Il telefono non viene rilevato in modo affidabile in modalità Find:** provare con l'hotspot Wi-Fi del telefono disattivato e ripetere più volte la ricerca a parità di condizioni. Controllare che indirizzo e nome Bluetooth salvati corrispondano al telefono desiderato e che `N.Phone` selezioni la voce corretta. Verificare che il modulo HC-05 riceva un'alimentazione stabile, in particolare se l'allarme funziona a batteria o tramite convertitori di tensione. Nel codice esaminato la ricerca Bluetooth utilizza per impostazione predefinita il percorso `V2`; non presumere che gli altri percorsi implementativi siano già stati validati sul dispositivo.

**L'allarme chiama un numero sbagliato o non chiama:** controllare `Phone`, `Ph.Altern.`, `N.Phone`, la SIM e il modulo GSM, nonché l'eventuale comando di inibizione delle chiamate (`Dc`). I comandi `P1`, `P2` e `Ck` sono utili per le prove, ricordando che effettuano chiamate reali.

**Le impostazioni scompaiono al riavvio:** verificare se la modifica è stata effettuata con un comando SMS temporaneo oppure tramite il menu di configurazione Bluetooth, che salva numerosi parametri in EEPROM. Controllare lo stato dell'inizializzazione EEPROM soltanto quando necessario.

**L'inserimento automatico dell'allarme è inatteso:** considerare il timer di cinque minuti e i suoi riavvii quando vengono aperti i menu Bluetooth. Verificare lo stato effettivo dell'allarme con una prova controllata.

## Hardware ed espansioni modulari

Il progetto nasce come piattaforma modulare: l'hardware dispone di tre porte di connessione USB per moduli e sensori esterni compatibili. Il materiale storico del progetto illustra anche possibili estensioni, come sensori di gas o fumo, rilevazione di fiamma, lettori di impronte digitali e NFC. **Si tratta di possibilità di espansione, non di funzionalità necessariamente implementate nel firmware attuale.**

## Documentazione

[Scarica il volantino di Portable Alarm System (DOCX)](Documents/Portable%20volantino.docx)

## Video

Dimostrazione del progetto:

https://www.youtube.com/watch?v=Jp0SQHU51y8

## Immagini del progetto

[![Portable Alarm System](SitoPortable/photo%20usate/portable1.png)](SitoPortable/photo%20usate/portable1.png)

[![Prototipo Portable Alarm System](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)

[![Dettaglio Portable Alarm System](SitoPortable/photo%20usate/terza.png)](SitoPortable/photo%20usate/terza.png)

## Contenuto del repository

Il repository contiene il firmware principale, uno sketch separato per l'inizializzazione EEPROM/Bluetooth, file di progettazione hardware, documentazione storica, immagini dimostrative e note di sviluppo. Alcuni componenti Bluetooth e delle librerie sono in corso di riorganizzazione: questa guida descrive i comportamenti osservati nel codice e non garantisce che ogni funzionalità sia supportata su tutte le revisioni hardware.

</details>

<details>
<summary><strong>English — User guide</strong></summary>

Portable Alarm System is a compact, modular alarm and monitoring device designed to protect objects, rooms, vehicles and other small environments. It detects alarm conditions and can call a configured phone number through its GSM module. Bluetooth provides local configuration and, in selected operating modes, detection of an authorised mobile phone.

The firmware is designed for an AVR/Arduino-compatible microcontroller and can work with external sensors and accessories.

> **Documentation status:** This usage guide describes the firmware examined on the `develop` branch on 23 October 2026. Some behaviour depends on the installed hardware, phone, Bluetooth module and firmware version. Verify safety-critical behaviour on the actual device before relying on the alarm.

## Quick start

1. Configure the authorised phone numbers and, if using a Find mode, the Bluetooth address and name of the selected phone. These settings are available through the Bluetooth **Configuration** menu.
2. Choose an operating mode: movement, external contact, PIR, or one of the two phone-detection (Find) modes.
3. Test the GSM call, the selected sensor and the Bluetooth search under the same power and phone conditions that will be used in practice.
4. Remember that the firmware includes an automatic alarm-arming timer of approximately **five minutes**. Access to some Bluetooth menu operations resets this timer. Do not assume that an `Alarm OFF` selection permanently disables future automatic arming.

The Bluetooth home menu reports **Alarm ON/OFF**, firmware version, temperature, an approximate battery level and `WhatzUp` (the code of the most recently recorded event). From the home menu you can open **Configuration** and **Security** or change the alarm state.

## Operating modes

| Mode | How it works | SMS command |
| --- | --- | --- |
| Movement / tilt | Detects internal movement or inclination and calls the selected phone number when armed. Bluetooth phone detection is not required. | `Md` |
| External contact, normally open | Uses the external contact input as an alarm source. | `Eo` |
| External contact, normally closed | Uses the external contact input as an alarm source. | `Ec` |
| PIR | Uses the external PIR sensor as an alarm source; an optional buzzer can sound. | `Wc` |
| **Find Mode 1** (phone-present suppression) | Searches for the authorised phone. When the phone is detected, the internal movement alarm is suppressed (and PIR calls are suppressed if PIR is independently enabled). Activating this mode via SMS resets other sensor modes, including PIR. The external-contact logic is separate. | `N1`–`N9` |
| **Find Mode 2** (phone-absence alarm) | Monitors for the authorised phone; failure to detect it can initiate a GSM call. The firmware's main loop excludes ordinary sensor alarms while this mode is active. | `F1`–`F9` |

For `N1`–`N9` and `F1`–`F9`, the digit selects **the number of phone-detection attempts per search cycle** (1 through 9). The same Bluetooth phone-detection path is used by both modes. The firmware may also make a call on activation of modes that invoke the alarm-activation routine, including `F1`–`F9`.

The code provides configuration entries for **two phone numbers** and **two sets of Bluetooth device identifiers**. The `N.Phone` setting selects the currently used phone (1 or 2); the current search logic checks the corresponding Bluetooth address and name rather than searching both phones at the same time.

### Important: mobile hotspot and Bluetooth Find reliability

> **Attention — potential interference on some phones:** With some mobile phones, enabling their **Wi-Fi hotspot/tethering** may make Bluetooth Find searches intermittent or prevent the alarm unit from detecting the authorised phone reliably. This can affect **both Find Mode 1 (`N1`–`N9`) and Find Mode 2 (`F1`–`F9`)** because both depend on detecting the phone. The observation concerns the **hotspot function**, not merely the mobile data connection or cellular network.
>
> If detection is unreliable, first **switch off the phone's hotspot** and repeat the test under otherwise identical conditions. Compare several searches with the hotspot off and on; also verify Bluetooth state, configured device name/address and stable power to the HC-05 module. The precise cause and the affected handset models have **not yet been conclusively established**, so this is a troubleshooting precaution, not a universal incompatibility.

In **Find Mode 1**, a missed detection can fail to suppress an otherwise valid movement alarm (or a separately enabled PIR alarm); in **Find Mode 2**, a missed detection can be interpreted as phone absence and initiate a call. Do not treat one successful discovery as sufficient proof of reliability.

## SMS control

The firmware accepts SMS commands from one of the two configured authorised phone numbers. Commands below are **case-sensitive** and exactly **two characters long** (except that the second character of `N1`–`N9` and `F1`–`F9` is a digit). Commands configure the **current runtime state**; do not assume they all persist across power cycles.

| Command | Operation |
| --- | --- |
| `N1`–`N9` | Activate Find Mode 1; digit = attempts per search cycle |
| `F1`–`F9` | Activate Find Mode 2; digit = attempts per search cycle |
| `Md` | Activate movement/tilt alarm without Bluetooth phone detection |
| `Eo` | Activate normally-open external-contact alarm |
| `Ec` | Activate normally-closed external-contact alarm |
| `Ex` | Disable external-contact alarm |
| `Wc` | Activate PIR alarm without Bluetooth phone detection |
| `Bz` | Enable optional buzzer (used for PIR alarm handling in the current firmware) |
| `P1`, `P2` | Select primary/alternative phone and initiate a call |
| `Ck` | Test by initiating a call to the selected number |
| `Rc` | Enable automatic answering of incoming calls |
| `Rs` | Disable automatic answering and initiate a call |
| `Dc` | Inhibit alarm calls and hang up the current call, if any |
| `Ab` | Turn on Bluetooth and open another configuration/arming-timer window |
| `Al` | Enable power-LED blinking |

These are the commands implemented by the inspected `listOfSmsCommands()` routine; this is **not** a list of every possible hardware or future firmware capability. Some mode-activation commands initiate an immediate GSM call as part of activation; perform setup with that behaviour in mind.

## Bluetooth menus

### Main menu

The main menu shows the current alarm state and offers **Alarm On / Alarm OFF**, **Configuration**, **Security**, temperature, battery level and the last event indicator (`WhatzUp`). In the current firmware the observed event letters include `M` (internal movement), `E` (external contact), `P` (PIR), `T` (high temperature), `V` (low voltage) and `X` (initial state).

### Configuration menu

- **Phone / Ph.Altern.** — main and alternative authorised telephone numbers.
- **N.Phone** — select phone 1 or phone 2 for calls and the associated Bluetooth detection entry.
- **TempMax / OffSetTemp** — temperature alarm threshold and sensor-calibration offset.
- **PIR status** — activate/deactivate the PIR input where the selected mode allows it.
- **Addr / Name / Addr2 / Name2** — configured Bluetooth identifiers for the selected phone entries (shown when a Find mode is active).
- **FindLoop / FindMode** — number of Bluetooth search attempts and operating mode (0 = no Find, 1 = Find Mode 1, 2 = Find Mode 2).
- **Ext.Int / Buzz.** — enable external interrupt and buzzer functions.

Changes made through many of these fields are saved to EEPROM and restored during firmware initialisation. By contrast, most SMS mode changes act on the current running state.

### Security menu

The security menu can change the Bluetooth **password** (after checking the existing password) and **device name**. Reconnect the phone after changing these settings, if necessary.

## Sensors, monitoring and notifications

The current firmware includes a movement/tilt interrupt, an external-contact interrupt, a PIR input, optional buzzer, an internal microcontroller temperature reading and a supply-voltage reading. The low-voltage threshold is set to approximately **3.25 V** in the examined firmware. High temperature and low voltage can trigger GSM calls independently of the ordinary motion-alarm path.

**Notifications are GSM calls**, not general-purpose outbound SMS messages. SMS is used mainly for remote command input. Make sure the SIM and GSM module can place calls and that the phone number selection is correct.

## Initial configuration and diagnostics

The repository contains a separate `Portable-Alarm-Initialized` sketch for EEPROM initialisation and Bluetooth diagnostics. Its `EEPROM_INITIALIZATION_ENABLED` option is **disabled by default** in the inspected branch. Enabling it causes EEPROM clearing and re-initialisation: back up required settings first and use the tool deliberately.

The main firmware prints `Firmware Git` and `Libraries Git` at startup, allowing hardware tests to be associated with their firmware and library revisions. The serial debug macros are controlled separately from the general Bluetooth menu and may be disabled in a normal build.

## Troubleshooting

**The phone is not reliably detected in Find mode:** Test with the phone's Wi-Fi hotspot switched off; repeat multiple times under identical conditions. Check that the saved Bluetooth address and name match the intended phone and that `N.Phone` selects the intended entry. Verify that the HC-05 is powered reliably, particularly when the alarm unit is battery-powered or uses voltage-conversion hardware. In the inspected source, Bluetooth discovery defaults to the `V2` implementation path; do not assume alternate implementation paths have been validated on your device.

**The alarm calls the wrong phone or does not call:** Check `Phone`, `Ph.Altern.`, `N.Phone`, the SIM/GSM module and any active call-inhibition command (`Dc`). `P1`, `P2` and `Ck` are useful checks, bearing in mind that they place a real call.

**Settings disappear after restarting:** Check whether the setting was made with an SMS runtime command or saved through the Bluetooth configuration menu. Inspect the EEPROM initialisation status only when necessary.

**Automatic arming behaves unexpectedly:** Be aware of the five-minute timer, including resets when the Bluetooth menus are opened. Validate the effective alarm state with a controlled test.

## Hardware and modular expansion

The project is intended as a modular platform: the hardware includes three USB connection ports for compatible external modules and sensors. The historical project material also explores possible extensions such as gas or smoke sensing, flame sensing, fingerprint readers and NFC. **These are expansion possibilities, not evidence that such modules are implemented by the current firmware.**

## Documentation

[Download the Portable Alarm System leaflet (DOCX)](Documents/Portable%20volantino.docx)

## Video

Project demonstration:

https://www.youtube.com/watch?v=Jp0SQHU51y8

## Project images

[![Portable Alarm System](SitoPortable/photo%20usate/portable1.png)](SitoPortable/photo%20usate/portable1.png)

[![Portable Alarm System prototype](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)

[![Portable Alarm System detail](SitoPortable/photo%20usate/terza.png)](SitoPortable/photo%20usate/terza.png)

## Repository contents

The repository contains the main firmware, a separate EEPROM/Bluetooth initialisation sketch, hardware design files, historical documentation, demonstration images and development notes. Some Bluetooth and library components are actively being refactored; this guide describes observed source behaviour rather than guaranteeing that every feature is supported on every hardware revision.

</details>
