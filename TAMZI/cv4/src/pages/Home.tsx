import {
  IonButton,
  IonCard,
  IonCardContent,
  IonCardHeader,
  IonCardSubtitle,
  IonCardTitle,
  IonCheckbox,
  IonButtons,
  IonContent,
  IonFooter,
  IonHeader,
  IonInput,
  IonItem,
  IonLabel,
  IonModal,
  IonNote,
  IonPage,
  IonText,
  IonTitle,
  IonToolbar,
} from '@ionic/react';
import { useEffect, useMemo, useState } from 'react';
import './Home.css';

const FORM_STORAGE_KEY = 'bmi-calculator-form';
const HISTORY_STORAGE_KEY = 'bmi-calculator-history';

type Gender = 'male' | 'female';

type FormState = {
  username: string;
  age: string;
  weight: string;
  height: string;
  gender: Gender;
};

type HistoryEntry = FormState & {
  bmi: string;
  category: string;
  createdAt: string;
};

const Home: React.FC = () => {
  const [username, setUsername] = useState('');
  const [age, setAge] = useState('');
  const [weight, setWeight] = useState('');
  const [height, setHeight] = useState('');
  const [gender, setGender] = useState<Gender>('male');
  const [history, setHistory] = useState<HistoryEntry[]>([]);
  const [isHistoryOpen, setIsHistoryOpen] = useState(false);
  const [hasLoadedStorage, setHasLoadedStorage] = useState(false);

  useEffect(() => {
    try {
      const savedForm = localStorage.getItem(FORM_STORAGE_KEY);

      if (savedForm) {
        const parsedForm = JSON.parse(savedForm) as Partial<FormState>;
        setUsername(parsedForm.username ?? '');
        setAge(parsedForm.age ?? '');
        setWeight(parsedForm.weight ?? '');
        setHeight(parsedForm.height ?? '');
        setGender(parsedForm.gender === 'female' ? 'female' : 'male');
      }

      const savedHistory = localStorage.getItem(HISTORY_STORAGE_KEY);

      if (savedHistory) {
        const parsedHistory = JSON.parse(savedHistory) as HistoryEntry[];
        setHistory(Array.isArray(parsedHistory) ? parsedHistory : []);
      }
    } catch {
      localStorage.removeItem(FORM_STORAGE_KEY);
      localStorage.removeItem(HISTORY_STORAGE_KEY);
    }

    setHasLoadedStorage(true);
  }, []);

  useEffect(() => {
    if (!hasLoadedStorage) {
      return;
    }

    localStorage.setItem(
      FORM_STORAGE_KEY,
      JSON.stringify({
        username,
        age,
        weight,
        height,
        gender,
      }),
    );
  }, [age, gender, hasLoadedStorage, height, username, weight]);

  useEffect(() => {
    if (!hasLoadedStorage) {
      return;
    }

    localStorage.setItem(HISTORY_STORAGE_KEY, JSON.stringify(history));
  }, [hasLoadedStorage, history]);

  const bmiData = useMemo(() => {
    const weightValue = Number(weight);
    const heightValue = Number(height);

    if (!weightValue || !heightValue) {
      return null;
    }

    const heightInMeters = heightValue / 100;
    const bmi = weightValue / (heightInMeters * heightInMeters);

    if (!Number.isFinite(bmi)) {
      return null;
    }

    let category = 'Obese';

    if (bmi < 18.5) {
      category = 'Underweight';
    } else if (bmi < 25) {
      category = 'Normal';
    } else if (bmi < 30) {
      category = 'Overweight';
    }

    return {
      bmi: bmi.toFixed(1),
      category,
    };
  }, [height, weight]);

  const clearForm = () => {
    setUsername('');
    setAge('');
    setWeight('');
    setHeight('');
    setGender('male');
    localStorage.removeItem(FORM_STORAGE_KEY);
  };

  const saveToHistory = () => {
    if (!bmiData) {
      return;
    }

    const nextEntry: HistoryEntry = {
      username,
      age,
      weight,
      height,
      gender,
      bmi: bmiData.bmi,
      category: bmiData.category,
      createdAt: new Date().toISOString(),
    };

    setHistory((currentHistory) => [nextEntry, ...currentHistory].slice(0, 10));
  };

  const clearHistory = () => {
    setHistory([]);
    localStorage.removeItem(HISTORY_STORAGE_KEY);
  };

  const deleteHistoryEntry = (createdAt: string) => {
    setHistory((currentHistory) =>
      currentHistory.filter((entry) => entry.createdAt !== createdAt),
    );
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar className="home-toolbar">
          <IonTitle>BMI Calculator</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen className="home-content">
        <div className="home-shell">
          <IonCard className="profile-card">
            <IonCardHeader>
              <IonCardTitle>Enter your details</IonCardTitle>
            </IonCardHeader>
            <IonCardContent>
              <div className="form-grid">
                <IonItem>
                  <IonLabel position="stacked">Username</IonLabel>
                  <IonInput
                    value={username}
                    placeholder="Your name"
                    onIonInput={(event) => setUsername(event.detail.value ?? '')}
                  />
                </IonItem>

                <IonItem>
                  <IonLabel position="stacked">Age</IonLabel>
                  <IonInput
                    type="number"
                    value={age}
                    placeholder="Years"
                    onIonInput={(event) => setAge(event.detail.value ?? '')}
                  />
                </IonItem>

                <IonItem>
                  <IonLabel position="stacked">Weight</IonLabel>
                  <IonInput
                    type="number"
                    value={weight}
                    placeholder="kg"
                    onIonInput={(event) => setWeight(event.detail.value ?? '')}
                  />
                </IonItem>

                <IonItem>
                  <IonLabel position="stacked">Height</IonLabel>
                  <IonInput
                    type="number"
                    value={height}
                    placeholder="cm"
                    onIonInput={(event) => setHeight(event.detail.value ?? '')}
                  />
                </IonItem>

                <IonItem>
                  <IonLabel position="stacked">Gender</IonLabel>
                  <div className="gender-checkbox-row">
                    <label className="gender-option">
                      <IonCheckbox
                        checked={gender === 'male'}
                        onIonChange={(event) => {
                          if (event.detail.checked) {
                            setGender('male');
                          }
                        }}
                      />
                      <IonText>
                        <p>Male</p>
                      </IonText>
                    </label>
                    <label className="gender-option">
                      <IonCheckbox
                        checked={gender === 'female'}
                        onIonChange={(event) => {
                          if (event.detail.checked) {
                            setGender('female');
                          }
                        }}
                      />
                      <IonText>
                        <p>Female</p>
                      </IonText>
                    </label>
                  </div>
                </IonItem>
              </div>

              <div className="card-actions">
                <IonButton expand="block" onClick={saveToHistory} disabled={!bmiData}>
                  Save to history
                </IonButton>
                <IonButton expand="block" fill="solid" color="medium" onClick={() => setIsHistoryOpen(true)}>
                  Open history
                </IonButton>
                <IonButton expand="block" onClick={clearForm} fill="outline">
                  Clear
                </IonButton>
              </div>
            </IonCardContent>
          </IonCard>

          <IonCard className="result-card">
            <IonCardHeader>
              <IonCardSubtitle>Result</IonCardSubtitle>
              <IonCardTitle>{username || 'Guest'}</IonCardTitle>
            </IonCardHeader>
            <IonCardContent>
              <div className="result-stat">
                <span className="result-label">BMI</span>
                <strong>{bmiData?.bmi ?? '--'}</strong>
              </div>

              <div className="result-details">
                <IonText>
                  <p>Age: {age || '--'}</p>
                </IonText>
                <IonText>
                  <p>Weight: {weight ? `${weight} kg` : '--'}</p>
                </IonText>
                <IonText>
                  <p>Height: {height ? `${height} cm` : '--'}</p>
                </IonText>
              </div>

              {bmiData ? (
                <IonNote color="primary">
                  Category: {bmiData.category}
                </IonNote>
              ) : (
                <IonNote color="medium">Enter weight and height to calculate BMI.</IonNote>
              )}
            </IonCardContent>
          </IonCard>

        </div>

        <IonModal isOpen={isHistoryOpen} onDidDismiss={() => setIsHistoryOpen(false)}>
          <IonHeader>
            <IonToolbar className="home-toolbar">
              <IonTitle>Saved history</IonTitle>
              <IonButtons slot="end">
                <IonButton onClick={() => setIsHistoryOpen(false)}>Close</IonButton>
              </IonButtons>
            </IonToolbar>
          </IonHeader>
          <IonContent className="history-modal-content">
            <div className="history-modal-body">
              {history.length ? (
                <div className="history-list">
                  {history.map((entry) => (
                    <div className="history-item" key={entry.createdAt}>
                      <div className="history-item-header">
                        <strong>{entry.username || 'Guest'}</strong>
                        <IonButton
                          fill="clear"
                          color="danger"
                          size="small"
                          onClick={() => deleteHistoryEntry(entry.createdAt)}
                        >
                          Delete
                        </IonButton>
                      </div>
                      <p>
                        BMI {entry.bmi} ({entry.category}) • {entry.weight} kg • {entry.height} cm
                      </p>
                      <p>
                        Age {entry.age || '--'} • Gender {entry.gender}
                      </p>
                      <p>{new Date(entry.createdAt).toLocaleString()}</p>
                    </div>
                  ))}
                </div>
              ) : (
                <IonNote color="medium">No saved history yet.</IonNote>
              )}
            </div>
          </IonContent>
          <IonFooter>
            <IonToolbar>
              <div className="history-modal-footer">
                <IonButton expand="block" fill="outline" onClick={clearHistory} disabled={!history.length}>
                  Clear history
                </IonButton>
              </div>
            </IonToolbar>
          </IonFooter>
        </IonModal>
      </IonContent>
    </IonPage>
  );
};

export default Home;
