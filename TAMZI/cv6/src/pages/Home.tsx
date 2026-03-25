import {
  IonContent,
  IonHeader,
  IonInput,
  IonItem,
  IonLabel,
  IonPage,
  IonSelect,
  IonSelectOption,
  IonSpinner,
  IonTitle,
  IonToolbar,
} from '@ionic/react';
import { useEffect, useMemo, useState } from 'react';
import './Home.css';

const CNB_URL = 'http://linedu.vsb.cz/~mor03/TAMZ/cnb_json.php';

type LanguageMode = 'browser' | 'en' | 'cs';
type EditedField = 'czk' | 'foreign';

type RawCurrency = {
  code?: string;
  unit?: string | number;
  rate?: string | number;
  country_label?: string;
  curr_label?: string;
};

type ApiResponse = {
  cached?: boolean;
  data?: RawCurrency[];
  date?: string;
  lang?: string;
  order?: string;
};

type CurrencyRecord = {
  code: string;
  unit: number;
  rate: number;
  countryLabel: string;
  currencyLabel: string;
  fullLabel: string;
};

const detectBrowserLanguage = (): 'en' | 'cs' => {
  if (typeof navigator === 'undefined') {
    return 'en';
  }

  return navigator.language.toLowerCase().startsWith('cs') ? 'cs' : 'en';
};

const parseNumber = (value: string): number | null => {
  const normalized = value.replace(',', '.').trim();

  if (!normalized) {
    return null;
  }

  const parsed = Number(normalized);
  return Number.isFinite(parsed) ? parsed : null;
};

const formatCalculatedValue = (value: number): string => {
  if (!Number.isFinite(value)) {
    return '';
  }

  return new Intl.NumberFormat('en-US', {
    maximumFractionDigits: 4,
    minimumFractionDigits: 0,
  }).format(value);
};

const normalizePayload = (payload: ApiResponse): { date: string; lang: string; currencies: CurrencyRecord[] } => {
  const currencies = (payload.data ?? [])
    .map((item) => {
      const code = item.code?.trim() ?? '';
      const unit = Number(item.unit);
      const rate = Number(item.rate);

      if (!code || !Number.isFinite(unit) || !Number.isFinite(rate) || unit <= 0 || rate <= 0) {
        return null;
      }

      const countryLabel = item.country_label?.trim() ?? code;
      const currencyLabel = item.curr_label?.trim() ?? code;

      return {
        code,
        unit,
        rate,
        countryLabel,
        currencyLabel,
        fullLabel: `${countryLabel} - ${currencyLabel}`,
      };
    })
    .filter((item): item is CurrencyRecord => item !== null)
    .sort((left, right) => left.code.localeCompare(right.code));

  return {
    date: payload.date ?? '',
    lang: payload.lang ?? '',
    currencies,
  };
};

const loadRates = async (url: string): Promise<ApiResponse> => {
  const response = await fetch(url);

  if (!response.ok) {
    throw new Error('Unable to load exchange rates.');
  }

  return (await response.json()) as ApiResponse;
};

const Home: React.FC = () => {
  const [date, setDate] = useState('');
  const [languageMode, setLanguageMode] = useState<LanguageMode>('browser');
  const [currencies, setCurrencies] = useState<CurrencyRecord[]>([]);
  const [selectedCode, setSelectedCode] = useState('EUR');
  const [rateDate, setRateDate] = useState('');
  const [sourceLabel, setSourceLabel] = useState('Current rates');
  const [feedLanguage, setFeedLanguage] = useState('');
  const [isCached, setIsCached] = useState(false);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState('');
  const [czkAmount, setCzkAmount] = useState('1000');
  const [foreignAmount, setForeignAmount] = useState('');
  const [lastEdited, setLastEdited] = useState<EditedField>('czk');

  const requestedLanguage = languageMode === 'browser' ? detectBrowserLanguage() : languageMode;
  const selectedCurrency =
    currencies.find((currency) => currency.code === selectedCode) ?? currencies[0] ?? null;
  const ratePerUnit = selectedCurrency ? selectedCurrency.rate / selectedCurrency.unit : null;

  useEffect(() => {
    let isCancelled = false;

    setIsLoading(true);
    setError('');

    const params = new URLSearchParams();
    params.set('lang', requestedLanguage);
    const requestUrl = new URL(CNB_URL);
    requestUrl.search = params.toString();

    const applyPayload = (payload: ApiResponse) => {
      const normalized = normalizePayload(payload);

      if (isCancelled) {
        return;
      }

      setCurrencies(normalized.currencies);
      setRateDate(normalized.date);
      setFeedLanguage(normalized.lang);
      setIsCached(Boolean(payload.cached));
      setSelectedCode((currentCode) =>
        normalized.currencies.some((currency) => currency.code === currentCode)
          ? currentCode
          : normalized.currencies[0]?.code ?? '',
      );
      setError('');
      setIsLoading(false);
    };

    const loadSnapshot = async (nextSourceLabel: string, nextErrorMessage: string) => {
      try {
        const payload = await loadRates(requestUrl.toString());
        applyPayload(payload);
        setSourceLabel(nextSourceLabel);
      } catch (loadError) {
        if (!isCancelled) {
          setError(loadError instanceof Error ? loadError.message : nextErrorMessage);
          setIsLoading(false);
        }
      }
    };

    if (date) {
      requestUrl.searchParams.set('date', date);
      setSourceLabel('JSON snapshot');
    } else {
      setSourceLabel('Current rates');
    }

    void loadSnapshot(
      date ? 'JSON snapshot' : 'Current rates',
      date ? 'Unable to load exchange rates for the selected date.' : 'Unable to load current exchange rates.',
    );

    return () => {
      isCancelled = true;
    };
  }, [date, requestedLanguage]);

  useEffect(() => {
    if (!selectedCurrency || lastEdited !== 'czk') {
      return;
    }

    const amount = parseNumber(czkAmount);
    if (amount === null) {
      setForeignAmount('');
      return;
    }

    setForeignAmount(formatCalculatedValue((amount * selectedCurrency.unit) / selectedCurrency.rate));
  }, [czkAmount, lastEdited, selectedCurrency]);

  useEffect(() => {
    if (!selectedCurrency || lastEdited !== 'foreign') {
      return;
    }

    const amount = parseNumber(foreignAmount);
    if (amount === null) {
      setCzkAmount('');
      return;
    }

    setCzkAmount(formatCalculatedValue((amount * selectedCurrency.rate) / selectedCurrency.unit));
  }, [foreignAmount, lastEdited, selectedCurrency]);

  const rateText = useMemo(() => {
    if (!selectedCurrency || ratePerUnit === null) {
      return 'Waiting for exchange rate data';
    }

    return `${selectedCurrency.rate.toFixed(3)} CZK / ${selectedCurrency.unit} ${selectedCurrency.code} (${ratePerUnit.toFixed(4)} CZK for 1 ${selectedCurrency.code})`;
  }, [ratePerUnit, selectedCurrency]);

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>CNB Currency Converter</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <div className="converter-page">
          <section className="hero-card">
            <p className="eyebrow">Czech National Bank feed</p>
            <h1>Convert CZK on the fly</h1>
          </section>

          <section className="panel-grid">
            <div className="panel">
              <h2>Feed Setup</h2>
              <IonItem lines="none" className="control-item">
                <IonLabel position="stacked">Date</IonLabel>
                <IonInput
                  type="date"
                  value={date}
                  onIonInput={(event) => setDate(String(event.detail.value ?? ''))}
                />
              </IonItem>

              <IonItem lines="none" className="control-item">
                <IonLabel position="stacked">Language</IonLabel>
                <IonSelect
                  interface="popover"
                  value={languageMode}
                  onIonChange={(event) => setLanguageMode(event.detail.value as LanguageMode)}
                >
                  <IonSelectOption value="browser">
                    Browser default ({detectBrowserLanguage()})
                  </IonSelectOption>
                  <IonSelectOption value="en">English</IonSelectOption>
                  <IonSelectOption value="cs">Czech</IonSelectOption>
                </IonSelect>
              </IonItem>

              <div className="status-block">
                <div>
                  <span className="status-label">Source</span>
                  <strong>{sourceLabel}</strong>
                </div>
                <div>
                  <span className="status-label">Rate date</span>
                  <strong>{rateDate || 'Loading'}</strong>
                </div>
                <div>
                  <span className="status-label">Feed lang</span>
                  <strong>{feedLanguage || requestedLanguage}</strong>
                </div>
                <div>
                  <span className="status-label">Cache</span>
                  <strong>{isCached ? 'cached' : 'fresh'}</strong>
                </div>
              </div>

              {isLoading ? (
                <div className="loading-row">
                  <IonSpinner name="crescent" />
                  <span>Loading exchange rates…</span>
                </div>
              ) : null}

              {error ? <p className="error-text">{error}</p> : null}
            </div>

            <div className="panel">
              <h2>Converter</h2>
              <p className="current-code">CUR: {selectedCurrency?.code ?? '---'}</p>
              <p className="rate-text">Exchange rate (CZK ↔ CUR) = {rateText}</p>

              <IonItem lines="none" className="control-item">
                <IonLabel position="stacked">CZK</IonLabel>
                <IonInput
                  inputmode="decimal"
                  value={czkAmount}
                  onIonInput={(event) => {
                    setLastEdited('czk');
                    setCzkAmount(String(event.detail.value ?? ''));
                  }}
                />
              </IonItem>

              <IonItem lines="none" className="control-item">
                <IonLabel position="stacked">{selectedCurrency?.code ?? 'CUR'}</IonLabel>
                <IonInput
                  inputmode="decimal"
                  value={foreignAmount}
                  onIonInput={(event) => {
                    setLastEdited('foreign');
                    setForeignAmount(String(event.detail.value ?? ''));
                  }}
                />
              </IonItem>
            </div>
          </section>

          <section className="panel">
            <div className="currency-heading">
              <h2>Choose Currency</h2>
              <p>Pick the target currency from the dropdown.</p>
            </div>
            <IonItem lines="none" className="control-item currency-select-item">
              <IonLabel position="stacked">Currency</IonLabel>
              <IonSelect
                interface="popover"
                value={selectedCode}
                placeholder="Select currency"
                className="currency-select"
                onIonChange={(event) => setSelectedCode(String(event.detail.value ?? ''))}
              >
                {currencies.map((currency) => (
                  <IonSelectOption key={currency.code} value={currency.code}>
                    {currency.code} - {currency.fullLabel}
                  </IonSelectOption>
                ))}
              </IonSelect>
            </IonItem>
          </section>
        </div>
      </IonContent>
    </IonPage>
  );
};

export default Home;
