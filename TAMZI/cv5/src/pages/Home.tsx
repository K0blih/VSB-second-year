import { FormEvent, useState } from 'react';
import {
  IonButton,
  IonContent,
  IonHeader,
  IonInput,
  IonItem,
  IonLabel,
  IonPage,
  IonSpinner,
  IonText,
  IonTitle,
  IonToolbar,
} from '@ionic/react';
import './Home.css';

const DEFAULT_URL = 'https://homel.vsb.cz/~mor03/TAMZ/TAMZ22.php';

const Home: React.FC = () => {
  const [username, setUsername] = useState('');
  const [scriptUrl, setScriptUrl] = useState(DEFAULT_URL);
  const [encodedToken, setEncodedToken] = useState('');
  const [decodedToken, setDecodedToken] = useState('');
  const [serverResponse, setServerResponse] = useState('');
  const [errorMessage, setErrorMessage] = useState('');
  const [isLoading, setIsLoading] = useState(false);

  const handleSubmit = async (event: FormEvent<HTMLFormElement>) => {
    event.preventDefault();

    const trimmedUsername = username.trim();
    const trimmedUrl = scriptUrl.trim();

    if (!trimmedUsername || !trimmedUrl) {
      setErrorMessage('Fill in both username and script URL.');
      return;
    }

    const timestamp = Date.now().toString();
    const firstRequestUrl = new URL(trimmedUrl);
    firstRequestUrl.searchParams.set('user', trimmedUsername);
    firstRequestUrl.searchParams.set('timestamp', timestamp);

    const secondRequestUrl = new URL(trimmedUrl);
    secondRequestUrl.searchParams.set('timestamp', timestamp);

    setIsLoading(true);
    setErrorMessage('');
    setEncodedToken('');
    setDecodedToken('');
    setServerResponse('');

    try {
      const tokenResponse = await fetch(firstRequestUrl.toString(), {
        headers: {
          Accept: 'text/plain',
        },
      });

      if (!tokenResponse.ok) {
        throw new Error(`Token request failed with status ${tokenResponse.status}.`);
      }

      const receivedEncodedToken = (await tokenResponse.text()).trim();
      const token = atob(receivedEncodedToken);
      setEncodedToken(receivedEncodedToken);
      setDecodedToken(token);

      const authenticatedResponse = await fetch(secondRequestUrl.toString(), {
        method: 'POST',
        headers: {
          Accept: 'text/plain',
          Authorization: `Bearer ${receivedEncodedToken}`,
        },
      });

      const authenticatedBody = await authenticatedResponse.text();

      if (!authenticatedResponse.ok) {
        throw new Error(
          `Authorized request failed with status ${authenticatedResponse.status}: ${authenticatedBody}`,
        );
      }

      setServerResponse(authenticatedBody);
    } catch (error) {
      setErrorMessage(
        error instanceof Error ? error.message : 'Unexpected error while contacting the script.',
      );
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>TAMZ Token Client</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">
          <IonToolbar>
            <IonTitle size="large">TAMZ Token Client</IonTitle>
          </IonToolbar>
        </IonHeader>
        <div className="token-page">
          <div className="token-card">
            <div className="token-copy">
              <h1>Token handshake</h1>
              <p>
                Send your login and current timestamp, decode the returned Base64 token with
                <code> atob()</code>, then call the script again with a Bearer token.
              </p>
            </div>

            <form className="token-form" onSubmit={handleSubmit}>
              <IonItem>
                <IonLabel position="stacked">Username</IonLabel>
                <IonInput
                  value={username}
                  onIonInput={(event) => setUsername(event.detail.value ?? '')}
                  placeholder="your-login"
                  required
                />
              </IonItem>

              <IonItem>
                <IonLabel position="stacked">Script URL</IonLabel>
                <IonInput
                  value={scriptUrl}
                  onIonInput={(event) => setScriptUrl(event.detail.value ?? '')}
                  type="url"
                  required
                />
              </IonItem>

              <IonButton className="submit-button" expand="block" type="submit" disabled={isLoading}>
                {isLoading ? <IonSpinner name="crescent" /> : 'Run Requests'}
              </IonButton>
            </form>

            <div className="results">
              <label className="result-field">
                <span>Encoded token</span>
                <input value={encodedToken} readOnly placeholder="Encoded token will appear here" />
              </label>

              <label className="result-field">
                <span>Decoded token</span>
                <input value={decodedToken} readOnly placeholder="Decoded token will appear here" />
              </label>

              <label className="result-field">
                <span>Authorized response</span>
                <textarea
                  value={serverResponse}
                  readOnly
                  placeholder="Response from the authenticated request"
                  rows={6}
                />
              </label>

              {errorMessage ? (
                <IonText color="danger">
                  <p className="status-message">{errorMessage}</p>
                </IonText>
              ) : null}
            </div>
          </div>
        </div>
      </IonContent>
    </IonPage>
  );
};

export default Home;
