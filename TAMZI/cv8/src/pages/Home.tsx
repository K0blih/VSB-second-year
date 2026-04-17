import {
  IonButton,
  IonContent,
  IonHeader,
  IonInput,
  IonPage,
  IonTitle,
  IonToolbar,
} from '@ionic/react';
import L from 'leaflet';
import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import 'leaflet/dist/leaflet.css';
import './Home.css';

const DEFAULT_TARGET = {
  latitude: 49.8356,
  longitude: 18.1633,
};

type PositionState = {
  latitude: number;
  longitude: number;
  accuracy: number;
  altitude: number | null;
  altitudeAccuracy: number | null;
  speed: number | null;
  heading: number | null;
  timestamp: number;
};

type TargetState = {
  latitude: string;
  longitude: string;
};

type GeoStatus = 'waiting' | 'active' | 'denied' | 'unavailable' | 'error';
type OrientationPermission = 'idle' | 'granted' | 'denied' | 'unsupported';

type MotionState = {
  speed: number | null;
  heading: number | null;
};

type RouteState = {
  coordinates: [number, number][];
  source: 'osrm' | 'direct';
};

const toRadians = (degrees: number) => (degrees * Math.PI) / 180;
const toDegrees = (radians: number) => (radians * 180) / Math.PI;
const normalizeDegrees = (degrees: number) => ((degrees % 360) + 360) % 360;

const formatNumber = (
  value: number | null | undefined,
  fractionDigits = 1,
  fallback = 'neni k dispozici',
) => {
  if (value === null || value === undefined || Number.isNaN(value)) {
    return fallback;
  }

  return value.toLocaleString('cs-CZ', {
    maximumFractionDigits: fractionDigits,
    minimumFractionDigits: fractionDigits,
  });
};

const formatCoordinate = (value: number | null | undefined) => {
  if (value === null || value === undefined || Number.isNaN(value)) {
    return 'neni k dispozici';
  }

  return value.toLocaleString('cs-CZ', {
    maximumFractionDigits: 6,
    minimumFractionDigits: 6,
  });
};

const getDistanceMeters = (
  fromLatitude: number,
  fromLongitude: number,
  toLatitude: number,
  toLongitude: number,
) => {
  const earthRadiusMeters = 6371000;
  const phi1 = toRadians(fromLatitude);
  const phi2 = toRadians(toLatitude);
  const deltaPhi = toRadians(toLatitude - fromLatitude);
  const deltaLambda = toRadians(toLongitude - fromLongitude);

  const a =
    Math.sin(deltaPhi / 2) * Math.sin(deltaPhi / 2) +
    Math.cos(phi1) *
      Math.cos(phi2) *
      Math.sin(deltaLambda / 2) *
      Math.sin(deltaLambda / 2);
  const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));

  return earthRadiusMeters * c;
};

const getInitialBearing = (
  fromLatitude: number,
  fromLongitude: number,
  toLatitude: number,
  toLongitude: number,
) => {
  const phi1 = toRadians(fromLatitude);
  const phi2 = toRadians(toLatitude);
  const deltaLambda = toRadians(toLongitude - fromLongitude);
  const y = Math.sin(deltaLambda) * Math.cos(phi2);
  const x =
    Math.cos(phi1) * Math.sin(phi2) -
    Math.sin(phi1) * Math.cos(phi2) * Math.cos(deltaLambda);

  return normalizeDegrees(toDegrees(Math.atan2(y, x)));
};

const getCardinalDirection = (heading: number | null | undefined) => {
  if (heading === null || heading === undefined || Number.isNaN(heading)) {
    return 'neznamy smer';
  }

  const directions = ['S', 'SV', 'V', 'JV', 'J', 'JZ', 'Z', 'SZ'];
  return directions[Math.round(normalizeDegrees(heading) / 45) % directions.length];
};

const getCompassHeading = (event: DeviceOrientationEvent) => {
  const compassHeading = (event as DeviceOrientationEvent & { webkitCompassHeading?: number })
    .webkitCompassHeading;

  if (typeof compassHeading === 'number') {
    return normalizeDegrees(compassHeading);
  }

  if (typeof event.alpha === 'number') {
    return normalizeDegrees(360 - event.alpha);
  }

  return null;
};

const getOrientationPermission = () =>
  DeviceOrientationEvent as typeof DeviceOrientationEvent & {
    requestPermission?: () => Promise<PermissionState>;
  };

const Home: React.FC = () => {
  const [now, setNow] = useState(() => new Date());
  const [position, setPosition] = useState<PositionState | null>(null);
  const [derivedMotion, setDerivedMotion] = useState<MotionState>({
    speed: null,
    heading: null,
  });
  const [fallbackAltitude, setFallbackAltitude] = useState<number | null>(null);
  const [altitudeSource, setAltitudeSource] = useState('Ceka se na data vysky.');
  const [route, setRoute] = useState<RouteState | null>(null);
  const [geoStatus, setGeoStatus] = useState<GeoStatus>('waiting');
  const [geoError, setGeoError] = useState('');
  const [orientationHeading, setOrientationHeading] = useState<number | null>(null);
  const [orientationStatus, setOrientationStatus] = useState('Kompas jeste neposlal data.');
  const [orientationPermission, setOrientationPermission] =
    useState<OrientationPermission>('idle');
  const [listenToOrientation, setListenToOrientation] = useState(false);
  const [target, setTarget] = useState<TargetState>({
    latitude: String(DEFAULT_TARGET.latitude),
    longitude: String(DEFAULT_TARGET.longitude),
  });
  const mapElementRef = useRef<HTMLDivElement | null>(null);
  const mapRef = useRef<L.Map | null>(null);
  const currentMarkerRef = useRef<L.Marker | null>(null);
  const targetMarkerRef = useRef<L.Marker | null>(null);
  const routeLayerRef = useRef<L.Polyline | null>(null);

  useEffect(() => {
    const timer = window.setInterval(() => setNow(new Date()), 1000);
    return () => window.clearInterval(timer);
  }, []);

  useEffect(() => {
    if (!('geolocation' in navigator)) {
      setGeoStatus('unavailable');
      setGeoError('Geolokace neni v tomto prohlizeci dostupna.');
      return undefined;
    }

    const watchId = navigator.geolocation.watchPosition(
      ({ coords, timestamp }) => {
        const nextPosition = {
          latitude: coords.latitude,
          longitude: coords.longitude,
          accuracy: coords.accuracy,
          altitude: coords.altitude,
          altitudeAccuracy: coords.altitudeAccuracy,
          speed: coords.speed,
          heading: coords.heading,
          timestamp,
        };

        setPosition((previousPosition) => {
          if (previousPosition) {
            const seconds = (timestamp - previousPosition.timestamp) / 1000;
            const distanceMeters = getDistanceMeters(
              previousPosition.latitude,
              previousPosition.longitude,
              nextPosition.latitude,
              nextPosition.longitude,
            );

            if (seconds > 0 && distanceMeters > 0.5) {
              setDerivedMotion({
                speed: distanceMeters / seconds,
                heading: getInitialBearing(
                  previousPosition.latitude,
                  previousPosition.longitude,
                  nextPosition.latitude,
                  nextPosition.longitude,
                ),
              });
            }
          }

          return nextPosition;
        });
        setGeoStatus('active');
        setGeoError('');
      },
      (error) => {
        setGeoStatus(error.code === error.PERMISSION_DENIED ? 'denied' : 'error');
        setGeoError(error.message || 'Nepodarilo se nacist polohu.');
      },
      {
        enableHighAccuracy: true,
        maximumAge: 1000,
        timeout: 15000,
      },
    );

    return () => navigator.geolocation.clearWatch(watchId);
  }, []);

  const requestCompass = useCallback(async () => {
    if (!('DeviceOrientationEvent' in window)) {
      setOrientationStatus('Kompas neni v tomto zarizeni dostupny.');
      setOrientationPermission('unsupported');
      return;
    }

    const orientation = getOrientationPermission();

    if (typeof orientation.requestPermission === 'function') {
      try {
        const permission = await orientation.requestPermission();
        setOrientationPermission(permission === 'granted' ? 'granted' : 'denied');

        if (permission !== 'granted') {
          setOrientationStatus('Povoleni kompasu bylo zamitnuto.');
          return;
        }
      } catch {
        setOrientationPermission('denied');
        setOrientationStatus('Povoleni kompasu se nepodarilo ziskat.');
        return;
      }
    } else {
      setOrientationPermission('granted');
    }

    setOrientationStatus('Ceka se na data kompasu.');
    setListenToOrientation(true);
  }, []);

  useEffect(() => {
    if (!listenToOrientation) {
      return undefined;
    }

    const handleOrientation = (event: DeviceOrientationEvent) => {
      const nextHeading = getCompassHeading(event);

      if (nextHeading === null) {
        setOrientationStatus('Kompas zatim neposkytuje kurz.');
        return;
      }

      setOrientationHeading(nextHeading);
      setOrientationStatus('Kurz z kompasu');
    };

    window.addEventListener('deviceorientation', handleOrientation, true);
    window.addEventListener('deviceorientationabsolute', handleOrientation, true);

    return () => {
      window.removeEventListener('deviceorientation', handleOrientation, true);
      window.removeEventListener('deviceorientationabsolute', handleOrientation, true);
    };
  }, [listenToOrientation]);

  useEffect(() => {
    if (!('DeviceOrientationEvent' in window)) {
      setOrientationStatus('Kompas neni v tomto zarizeni dostupny.');
      setOrientationPermission('unsupported');
      return;
    }

    const orientation = getOrientationPermission();

    if (typeof orientation.requestPermission !== 'function') {
      void requestCompass();
    }
  }, [requestCompass]);

  const currentHeading = orientationHeading ?? position?.heading ?? derivedMotion.heading;
  const currentHeadingSource =
    orientationHeading !== null
      ? orientationStatus
      : position?.heading !== null && position?.heading !== undefined
        ? 'Kurz z GPS'
        : derivedMotion.heading !== null
          ? 'Kurz vypocitany z pohybu'
          : orientationStatus;
  const currentSpeed = position?.speed ?? derivedMotion.speed;
  const currentSpeedSource =
    position?.speed !== null && position?.speed !== undefined
      ? 'Rychlost z GPS'
      : derivedMotion.speed !== null
        ? 'Rychlost vypocitana z pohybu'
        : 'Pro vypocet rychlosti je potreba zmena polohy.';

  const parsedTarget = useMemo(() => {
    const latitude = Number.parseFloat(target.latitude.replace(',', '.'));
    const longitude = Number.parseFloat(target.longitude.replace(',', '.'));

    if (
      Number.isNaN(latitude) ||
      Number.isNaN(longitude) ||
      latitude < -90 ||
      latitude > 90 ||
      longitude < -180 ||
      longitude > 180
    ) {
      return null;
    }

    return { latitude, longitude };
  }, [target]);

  const navigation = useMemo(() => {
    if (!position || !parsedTarget) {
      return null;
    }

    const distanceMeters = getDistanceMeters(
      position.latitude,
      position.longitude,
      parsedTarget.latitude,
      parsedTarget.longitude,
    );
    const bearing = getInitialBearing(
      position.latitude,
      position.longitude,
      parsedTarget.latitude,
      parsedTarget.longitude,
    );
    const relativeBearing =
      currentHeading === null ? null : normalizeDegrees(bearing - currentHeading);

    return {
      distanceMeters,
      bearing,
      relativeBearing,
    };
  }, [currentHeading, parsedTarget, position]);

  const displayedAltitude = position?.altitude ?? fallbackAltitude;
  const displayedAltitudeSource =
    position?.altitude !== null && position?.altitude !== undefined
      ? 'Vyska z GPS'
      : fallbackAltitude !== null
        ? altitudeSource
        : 'Vyska zatim neni k dispozici.';

  useEffect(() => {
    if (!position || position.altitude !== null) {
      return undefined;
    }

    const controller = new AbortController();
    const timeoutId = window.setTimeout(() => controller.abort(), 6000);

    const loadElevation = async () => {
      try {
        const response = await fetch(
          `https://api.open-elevation.com/api/v1/lookup?locations=${position.latitude},${position.longitude}`,
          { signal: controller.signal },
        );

        if (!response.ok) {
          throw new Error('Elevation request failed');
        }

        const data = (await response.json()) as {
          results?: Array<{ elevation?: number }>;
        };
        const elevation = data.results?.[0]?.elevation;

        if (typeof elevation === 'number') {
          setFallbackAltitude(elevation);
          setAltitudeSource('Vyska z online modelu terenu');
        }
      } catch {
        setAltitudeSource('GPS ani online model vysku neposkytly.');
      } finally {
        window.clearTimeout(timeoutId);
      }
    };

    void loadElevation();

    return () => {
      window.clearTimeout(timeoutId);
      controller.abort();
    };
  }, [position]);

  useEffect(() => {
    if (!position || !parsedTarget) {
      setRoute(null);
      return undefined;
    }

    const controller = new AbortController();
    const timeoutId = window.setTimeout(() => controller.abort(), 8000);

    const loadRoute = async () => {
      const directRoute: RouteState = {
        coordinates: [
          [position.latitude, position.longitude],
          [parsedTarget.latitude, parsedTarget.longitude],
        ],
        source: 'direct',
      };

      try {
        const response = await fetch(
          `https://router.project-osrm.org/route/v1/foot/${position.longitude},${position.latitude};${parsedTarget.longitude},${parsedTarget.latitude}?overview=full&geometries=geojson`,
          { signal: controller.signal },
        );

        if (!response.ok) {
          throw new Error('Route request failed');
        }

        const data = (await response.json()) as {
          routes?: Array<{ geometry?: { coordinates?: [number, number][] } }>;
        };
        const coordinates = data.routes?.[0]?.geometry?.coordinates;

        if (!coordinates?.length) {
          throw new Error('Route response has no coordinates');
        }

        setRoute({
          coordinates: coordinates.map(([longitude, latitude]) => [latitude, longitude]),
          source: 'osrm',
        });
      } catch {
        setRoute(directRoute);
      } finally {
        window.clearTimeout(timeoutId);
      }
    };

    void loadRoute();

    return () => {
      window.clearTimeout(timeoutId);
      controller.abort();
    };
  }, [parsedTarget, position]);

  useEffect(() => {
    if (!mapElementRef.current || mapRef.current) {
      return;
    }

    const map = L.map(mapElementRef.current, {
      zoomControl: true,
    }).setView([DEFAULT_TARGET.latitude, DEFAULT_TARGET.longitude], 14);

    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: '&copy; OpenStreetMap contributors',
      maxZoom: 19,
    }).addTo(map);

    mapRef.current = map;
  }, []);

  useEffect(() => {
    const map = mapRef.current;

    if (!map || !position || !parsedTarget) {
      return;
    }

    const currentLatLng: [number, number] = [position.latitude, position.longitude];
    const targetLatLng: [number, number] = [parsedTarget.latitude, parsedTarget.longitude];
    const currentIcon = L.divIcon({
      className: 'map-marker current-marker',
      html: '<span>Ja</span>',
    });
    const targetIcon = L.divIcon({
      className: 'map-marker target-marker',
      html: '<span>VSB</span>',
    });

    if (!currentMarkerRef.current) {
      currentMarkerRef.current = L.marker(currentLatLng, { icon: currentIcon }).addTo(map);
    } else {
      currentMarkerRef.current.setLatLng(currentLatLng);
    }

    if (!targetMarkerRef.current) {
      targetMarkerRef.current = L.marker(targetLatLng, { icon: targetIcon }).addTo(map);
    } else {
      targetMarkerRef.current.setLatLng(targetLatLng);
    }

    const routeCoordinates = route?.coordinates ?? [currentLatLng, targetLatLng];

    if (!routeLayerRef.current) {
      routeLayerRef.current = L.polyline(routeCoordinates, {
        color: '#d22730',
        opacity: 0.9,
        weight: 5,
      }).addTo(map);
    } else {
      routeLayerRef.current.setLatLngs(routeCoordinates);
    }

    const bounds = L.latLngBounds(routeCoordinates);
    map.fitBounds(bounds.pad(0.2), { maxZoom: 16 });
    window.setTimeout(() => map.invalidateSize(), 50);
  }, [parsedTarget, position, route]);

  const updateTarget = (field: keyof TargetState, value: string | number | null | undefined) => {
    setTarget((current) => ({
      ...current,
      [field]: String(value ?? ''),
    }));
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>GPS Dashboard</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">
          <IonToolbar>
            <IonTitle size="large">GPS Dashboard</IonTitle>
          </IonToolbar>
        </IonHeader>

        <main className="dashboard">
          <section className="hero-section">
            <h1>Dashboard polohy a navigace</h1>
            <p>
              Aktualni cas, poloha, vyska, kurz a navadeni ke zvolenemu bodu.
            </p>
          </section>

          <section className="status-grid" aria-label="Zakladni informace">
            <article className="info-card">
              <span>Datum</span>
              <strong>{now.toLocaleDateString('cs-CZ')}</strong>
            </article>
            <article className="info-card">
              <span>Cas</span>
              <strong>{now.toLocaleTimeString('cs-CZ')}</strong>
            </article>
            <article className="info-card">
              <span>Stav polohy</span>
              <strong>
                {geoStatus === 'active'
                  ? 'Aktivni'
                  : geoStatus === 'waiting'
                    ? 'Ceka se na povoleni'
                    : geoStatus === 'denied'
                      ? 'Povoleni zamitnuto'
                      : 'Nedostupne'}
              </strong>
            </article>
          </section>

          {geoError && <p className="alert">{geoError}</p>}

          <section className="dashboard-grid">
            <article className="panel position-panel">
              <div className="panel-heading">
                <p className="eyebrow">GPS</p>
                <h2>Zemepisna poloha</h2>
              </div>

              <dl className="data-list">
                <div>
                  <dt>Sirka</dt>
                  <dd>{formatCoordinate(position?.latitude)}</dd>
                </div>
                <div>
                  <dt>Delka</dt>
                  <dd>{formatCoordinate(position?.longitude)}</dd>
                </div>
                <div>
                  <dt>Tolerance</dt>
                  <dd>
                    {position
                      ? `${formatNumber(position.accuracy, 0)} m`
                      : 'neni k dispozici'}
                  </dd>
                </div>
                <div>
                  <dt>Nadmorska vyska</dt>
                  <dd>
                    {displayedAltitude === null || displayedAltitude === undefined
                      ? 'neni k dispozici'
                      : `${formatNumber(displayedAltitude, 1)} m`}
                  </dd>
                </div>
                <div>
                  <dt>Zdroj vysky</dt>
                  <dd>{displayedAltitudeSource}</dd>
                </div>
                <div>
                  <dt>Rychlost</dt>
                  <dd>
                    {currentSpeed === null || currentSpeed === undefined
                      ? 'neni k dispozici'
                      : `${formatNumber(currentSpeed * 3.6, 1)} km/h`}
                  </dd>
                </div>
                <div>
                  <dt>Zdroj rychlosti</dt>
                  <dd>{currentSpeedSource}</dd>
                </div>
                <div>
                  <dt>Aktualizace</dt>
                  <dd>
                    {position
                      ? new Date(position.timestamp).toLocaleTimeString('cs-CZ')
                      : 'neni k dispozici'}
                  </dd>
                </div>
              </dl>
            </article>

            <article className="panel compass-panel">
              <div className="panel-heading">
                <p className="eyebrow">Kompas</p>
                <h2>Kurz</h2>
              </div>

              <div className="compass" aria-label="Graficky kompas">
                <div className="degree-mark north">0</div>
                <div className="degree-mark east">90</div>
                <div className="degree-mark south">180</div>
                <div className="degree-mark west">270</div>
                <div
                  className="compass-needle"
                  style={{ transform: `rotate(${currentHeading ?? 0}deg)` }}
                >
                  <span />
                </div>
                <div className="compass-center" />
              </div>

              <p className="heading-value">
                {currentHeading === null || currentHeading === undefined
                  ? 'Kurz neni k dispozici'
                  : `${formatNumber(currentHeading, 0)} deg ${getCardinalDirection(currentHeading)}`}
              </p>
              <p className="muted">{currentHeadingSource}</p>
              {orientationPermission === 'idle' && (
                <IonButton className="reset-button compass-button" onClick={requestCompass}>
                  Povolit kompas
                </IonButton>
              )}
            </article>

            <article className="panel navigation-panel">
              <div className="panel-heading">
                <p className="eyebrow">Navigace</p>
                <h2>Navadeni k bodu</h2>
              </div>

              <div className="target-inputs">
                <IonInput
                  label="Cilova sirka"
                  labelPlacement="stacked"
                  type="number"
                  inputMode="decimal"
                  value={target.latitude}
                  onIonInput={(event) => updateTarget('latitude', event.detail.value)}
                />
                <IonInput
                  label="Cilova delka"
                  labelPlacement="stacked"
                  type="number"
                  inputMode="decimal"
                  value={target.longitude}
                  onIonInput={(event) => updateTarget('longitude', event.detail.value)}
                />
              </div>

              <IonButton
                className="reset-button"
                fill="outline"
                onClick={() =>
                  setTarget({
                    latitude: String(DEFAULT_TARGET.latitude),
                    longitude: String(DEFAULT_TARGET.longitude),
                  })
                }
              >
                VSB Ostrava
              </IonButton>

              {!parsedTarget && <p className="alert">Zadejte platnou sirku a delku cile.</p>}

              <dl className="data-list">
                <div>
                  <dt>Vzdalenost</dt>
                  <dd>
                    {navigation
                      ? navigation.distanceMeters >= 1000
                        ? `${formatNumber(navigation.distanceMeters / 1000, 2)} km`
                        : `${formatNumber(navigation.distanceMeters, 0)} m`
                      : 'neni k dispozici'}
                  </dd>
                </div>
                <div>
                  <dt>Azimut k cili</dt>
                  <dd>
                    {navigation
                      ? `${formatNumber(navigation.bearing, 0)} deg ${getCardinalDirection(navigation.bearing)}`
                      : 'neni k dispozici'}
                  </dd>
                </div>
                <div>
                  <dt>Navadeni</dt>
                  <dd>
                    {!navigation
                      ? 'neni k dispozici'
                      : navigation.relativeBearing === null
                        ? `Jdete podle azimutu ${formatNumber(navigation.bearing, 0)} deg`
                        : `Otocit o ${formatNumber(navigation.relativeBearing, 0)} deg`}
                  </dd>
                </div>
              </dl>
            </article>

            <article className="panel map-panel">
              <div className="panel-heading">
                <p className="eyebrow">Mapa</p>
                <h2>Trasa k VSB Ostrava</h2>
              </div>

              <div ref={mapElementRef} className="map-view" aria-label="Mapa trasy" />
              <p className="muted map-status">
                {!position
                  ? 'Mapa ceka na aktualni polohu.'
                  : route?.source === 'osrm'
                    ? 'Trasa je vypocitana podle cest.'
                    : 'Zobrazuje se prima spojnice, dokud se nenacte trasa podle cest.'}
              </p>
            </article>
          </section>
        </main>
      </IonContent>
    </IonPage>
  );
};

export default Home;
