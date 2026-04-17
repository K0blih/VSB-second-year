# android build
in project root:
npm run build
npx cap sync android
cd android
./gradlew assembleDebug

# run locally in web
npm run dev

# locally on local network
npm run dev -- --host 0.0.0.0