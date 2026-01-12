
# OpenHaldex - ESP32 C6 (Canadian French/Français Canadien)

EN:
An open-source Generation 1, 2 & 4 Haldex Controller which originates/is a fork from ABangingDonk's 'OpenHaldex T4'.  It has been extended into Gen2 and Gen4 variants, with Gen3 and Gen5 currently unsupported - CAN reads of these systems would be awesome! Originally based on the Teensy 4.0; the ESP32 features two TWAI (CAN) interfaces as well as WiFi and Bluetooth support; which makes it an ideal candidate for easier interfacing.

FR:
Un contrôleur à source ouverte Haldex Générations 1, 2 et 4 qui provient/est un "fork" de ABangingDonk's "OpenHaldex T4". Ce dernier a été étendu pour les variantes Gen2 et Gen4, les Gen3 et 5 sont présentement non pris en charge; des lectures CAN de ces systèmes serait super! Initialement fondé sur le Teensy 4.0; l'ESP32 comprend deux interfaces TWAI (CAN) ainsi que le support WiFi et Bluetooth, ce qui le rend un candidat idéal pour un interfaçage facile.

![OpenHaldex-C6](/Images/BoardOverview.png)

### Concept

EN:
The basis of the module is to act as a middle man - read the incoming CAN frames destined for the OEM Haldex controller, and, if required, adjust these messages to 'trick' the Haldex into thinking more (or less) lock is required.  Generation 1, 2 and 4 are all available as 'standalone' systems - which means that no other ECUs have to be present and OpenHaldex will create the necessary frames for Haldex operation.

FR:
La base du module est d'agir comme intermédiaire - il lit les trames CAN entrantes destinées au contrôleur Haldex d'origine, et, si nécessaire, ajuste ces messages pour 'tromper' le Haldex en lui faisant croire qu'un verrouillage plus (ou moins) important est requis. Les générations 1,2 et 4 sont tous disponibles en tant que systèmes dits indépendants, ce qui signifie qu'aucun autre ECU a besoin d'être présent et OpenHaldex crééra les trames nécessaires pour l'opération du Haldex.

### ESP32 C6

EN:
The ESP32 C6 features two TWAI controllers - which allows CANBUS messages to be read, processed and re-transmitted towards the Haldex.  It also supports WiFi and Bluetooth - which makes on-the-fly configuration changes possible.  The original Teensy had an external HC-05 Bluetooth chip which is limited support wise. 

FR:
L'ESP32 C6 comprend deux contrôleurs TWAI, ce qui permet de lire, traiter et retransmettre les trames CANBUS vers le Haldex. Il supporte aussi le WiFi et Bluetooth, ce qui rend les changements de configuration à la volée possible. Le Teensy original avait une puce Bluetooth HC-05 externe qui est limitée en matière de support.

### The Modes/Les Modes

EN:
The controller allows for 4 main modes: Stock (act as OEM), FWD (zero lock), 7525 (some lock) and 5050 (100% lock) at the Haldex differential.  Generation 1, 2 and 4 have been tested on the bench to allow for a full understanding of what the stock CAN messages look like & therefore what messages need to be editted / created. These modes are displayed as colours using the 5mm PCB LED: Red (Stock), Green (FWD), Cyan (7525) and Blue (5050). Custom modes are purple. The modes can be toggled with the onboard 'Mode' button or changed via. WiFi. Disabling at low throttle inputs or high speed inputs are also configurable.

FR:
Le contrôleur permet quatre modes principaux : Stock (agir comme d'origine), FWD (aucun verrouillage), 7525 (verrouillage partiel) et 5050 (verrouillage à 100 %) au différentiel Haldex. Les générations 1, 2 et 4 ont été testées sur banc d'essai pour permettre une compréhension complète de l'apparence des messages CAN d'origine et donc des messages qui doivent être modifiés ou créés. Ces modes sont affichés sous forme de couleurs à l'aide de la DEL PCB de 5 mm : Rouge (Stock), Vert (FWD), Cyan (7525) et Bleu (5050). Les modes personnalisés sont mauves. Les modes peuvent être basculés avec le bouton « Mode » intégré ou modifiés via WiFi. Une désactivation aux faibles accélérations ou aux vitesses élevées est également configurable.

### WiFi Setup/Configuration WiFi

EN:
WiFi setup and configuration is always active.  Connect to 'OpenHaldexC6' by searching in WiFi devices and searching for 192.168.1.1 in a browser.  All settings are available for editing.  Should the WiFi page hang, a long press on the 'mode' button will reset the WiFi connection.

FR:
La configuration WiFi est toujours active. Connectez-vous à « OpenHaldexC6 » en cherchant dans les appareils WiFi et en accédant à 192.168.1.1 dans un navigateur. Tous les paramètres sont disponibles pour modification. Si la page WiFi se fige, un appui prolongé sur le bouton « mode » réinitialisera la connexion WiFi.

### Pinouts

| Pin/Broche | Signal | Notes |
|-----|--------|-------|
| 1 | Vbatt | 12 V |
| 2 | Ground/MALT | — |
| 3 | Chassis CAN Low | — |
| 4 | Chassis CAN High | — |
| 5 | Haldex CAN Low | — |
| 6 | Haldex CAN High | — |
| 7 | Switch Mode External/Commutateur de mode externe | +12 V to activate/pour activer |
| 8 | Brake Switch In/Entrée commutateur de frein | +12 V to activate/pour activer |
| 9 | Brake Switch Out/Sortie commutateur de frein | — |
| 10 | Handbrake Switch In/Entrée commutateur de frein à main | +12 V to activate/pour activer |
| 11 | Handbrake Switch Out/Sortie commutateur de frein à main | — |


### Uploading Code/Téléverser du code

EN:
For users wishing to customise or edit the code, it is released here for free use.  Connect the Haldex controller via. a data USB-C cable (note some are ONLY power, so this needs to be checked). It is recommended to check [the original repo](https://github.com/adamforbes92/OpenHaldex-C6) regularly for updates, as the French translations might be delayed.  An Over-The-Air (OTA) updater would be cool to implement!

FR:
Pour les utilisateurs souhaitant personnaliser ou modifier le code, celui-ci est publié ici pour utilisation gratuite. Connectez le contrôleur Haldex via un câble USB-C de données (notez que certains sont SEULEMENT pour l'alimentation, donc cela doit être vérifié). Il est recommandé de consulter régulièrement [le dépôt original](https://github.com/adamforbes92/OpenHaldex-C6) (en anglais) pour les mises à jour, car les traductions françaises pourraient être retardées. Un système de mise à jour par liaison radio (OTA) serait génial à implémenter !

### The PCB & Enclosure/Le PCB et boîtier

EN:
The Gerber files for the PCB, should anyone wish to build their own, is under the "PCB" folder.  This is the latest board.
Similarly, the enclosures are also here.

>Pinout & functionality remains the same for ALL generations of enclosure.

FR:
Les fichiers Gerber pour le PCB, si quelqu'un souhaite construire le sien, se trouvent dans le dossier « PCB ». Il s'agit de la carte la plus récente.
De même, les boîtiers sont également ici.

>Le pinout et les fonctionnalités restent identiques pour TOUTES les générations de boîtier.

![OpenHaldex-C6](/Images/BoardTop.png)

![OpenHaldex-C6](/Images/BoardBottom.png)

### Nice to Haves/Fonctionanalités supplémentaires

EN:
The board supports broadcasting the Haldex output via. CAN - which allows pairing with the FIS controller to capture (and received) current and new modes. Flashing LED if there is an issue with writing CAN messages. Follow Brake/Handbrake signals (via. hard-wired signals).  The Haldex controller can 'pass-through' the brake/handbrake signals to enable/disable the controller or they can be ignored so that it is always active.

FR:
La carte prend en charge la diffusion de la sortie Haldex via CAN - ce qui permet le jumelage avec le contrôleur FIS pour capturer (et recevoir) les modes actuels et nouveaux. DEL clignotante s'il y a un problème avec l'écriture des messages CAN. Suivre les signaux de frein/frein à main (via signaux câblés). Le contrôleur Haldex peut « transmettre » les signaux de frein/frein à main pour activer/désactiver le contrôleur, ou ils peuvent être ignorés pour qu'il soit toujours actif.

### Shoutouts/Dédicaces

EN:
Massive thanks to Arwid Vasilev for re-designing the PCB!

FR:
Un immense merci à Arwid Vasilev pour avoir reconçu le PCB!

## Disclaimer/Mise en garde

EN:
It should be noted that this will modify Haldex operation and therefore can only be operated off-road and on a closed course. It should always be assumed that the unit may crash/hang or cause the Haldex to operate unprediably and caution should be exercised when in use. Using this unit in any way will exert more strain on drivetrain components, and while the OEM safety features are still in place, it should be understood that having the Haldex unit locked up permanently may cause acceleated wear. 

Forbes-Automotive or LVT Technologies takes no responsiblity for damages as a result of using this unit or code in any form.

FR:
Il convient de noter que ceci modifiera le fonctionnement du Haldex et ne peut donc être utilisé que hors route et sur circuit fermé. Il faut toujours présumer que l'appareil peut se bloquer ou causer un fonctionnement imprévisible du Haldex, et la prudence doit être exercée lors de l'utilisation. L'utilisation de cet appareil de quelque manière que ce soit exercera plus de contraintes sur les composants de la chaîne cinématique, et bien que les dispositifs de sécurité d'origine soient toujours en place, il faut comprendre que le verrouillage permanent du Haldex peut causer une usure accélérée.

Forbes-Automotive ou LVT Technologies n'assument aucune responsabilité pour les dommages résultant de l'utilisation de cet appareil ou de ce code sous quelque forme que ce soit.
