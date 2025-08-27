# MX Daily Check-In

A classroom tool for generating Ontario curriculum-aligned daily check-in questions with AI assistance and Social-Emotional Learning (SEL) integration.

## Features

- **Ontario Curriculum Alignment**: Questions aligned with Ontario curriculum strands and topics
- **SEL Integration**: Social-Emotional Learning focus areas based on Second Step curriculum
- **AI-Powered**: Uses Google Gemini AI to generate contextual questions
- **Multiple Response Modes**: 2, 3, 4 choice, open-ended, and "Would You Rather?" formats
- **Student Management**: Roster management with drag-and-drop functionality
- **Auto-Backup**: Automatic switching between multiple API keys when rate limits are hit

## Setup

### 1. Get Google AI API Key

1. Go to [Google AI Studio](https://aistudio.google.com/)
2. Create a new project or select existing
3. Get your API key from the API keys section
4. Copy the API key

### 2. Configure API Keys

Open `Qfinal_github.html` and replace the placeholder API keys:

```javascript
const API_KEYS = {
  main: 'YOUR_MAIN_API_KEY_HERE',    // Replace with your actual API key
  BU1: 'YOUR_BU1_API_KEY_HERE',      // Optional: Add backup key 1
  BU2: 'YOUR_BU2_API_KEY_HERE'       // Optional: Add backup key 2
};
```

### 3. Open the Application

Simply open `Qfinal_github.html` in your web browser. No server setup required!

## Usage

### Basic Operation

1. **Open Settings** (S key or Settings button)
2. **Configure Filters**:
   - Grade level (1-6)
   - Subject (Math, Language, Science, etc.)
   - Bloom's Taxonomy level
   - Learning Skills focus
   - SEL focus (optional)
   - Keywords (optional)
3. **Generate Question** (G key or Generate button)
4. **Customize Responses** if needed
5. **Use in Classroom**

### Response Modes

- **2/3/4 Choice**: Multiple choice with specified number of options
- **Open**: Single open-ended response
- **Would You Rather**: Two-choice comparison questions

### Student Management

- **Edit Roster**: Add/remove students
- **Drag & Drop**: Move students between response zones
- **Partner Groups**: Automatic grouping functionality
- **Parking Lot**: Temporary holding area for students

## Ontario Curriculum Integration

The tool includes comprehensive Ontario curriculum data:

- **Math**: Number, Patterning & Algebra, Measurement, Geometry, Data
- **Language**: Oral Communication, Reading, Writing, Media Literacy
- **Science**: Life Systems, Matter & Energy, Structures & Mechanisms, Earth & Space
- **Social Studies**: Heritage & Identity, People & Environments
- **Arts**: Visual Arts, Music, Drama, Dance
- **Phys-Ed**: Active Living, Movement Skills, Healthy Living

## SEL (Social-Emotional Learning) Features

Six SEL domains with grade-appropriate topics:

1. **Skills for Learning**: Focus, attention, self-talk, goal-setting
2. **Empathy**: Feelings identification, perspective-taking, caring actions
3. **Emotion Management**: Calm-down steps, body signals, stress management
4. **Problem Solving**: STEP method (Say, Think, Explore, Pick)
5. **Bullying Prevention**: Recognize/report/refuse, upstander skills
6. **Growth Mindset**: Yet language, effort talk, strategy reflection

## Keyboard Shortcuts

- **S**: Open Settings
- **G**: Generate new question
- **R**: Reset/Quick reset
- **P**: Partner grouping
- **Arrow Keys**: Navigate group size and timer

## API Key Management

The tool supports multiple API keys with automatic failover:

- **Main Key**: Primary API key for question generation
- **Backup Keys**: Automatic switching when rate limits are hit
- **Token Tracking**: Visual progress bars showing remaining API calls
- **Rate Limit Handling**: Automatic detection and switching

## Browser Compatibility

Works in all modern browsers:
- Chrome (recommended)
- Firefox
- Safari
- Edge

## Local Storage

All settings and student data are stored locally in your browser:
- No data sent to external servers (except AI API calls)
- Settings persist between sessions
- Student rosters saved automatically

## Privacy & Security

- **Local Storage**: All data stays on your device
- **No User Accounts**: No login required
- **API Keys**: Stored locally in browser storage
- **No Analytics**: No tracking or data collection

## Troubleshooting

### AI Not Working
1. Check your API key is correct
2. Verify you have remaining API calls (200/day free tier)
3. Try switching to a backup key if available
4. Check browser console for error messages

### Students Not Saving
1. Ensure browser supports local storage
2. Try refreshing the page
3. Check browser console for errors

### Questions Too Long
1. Enable "Concise" mode in settings
2. Use shorter keywords
3. Select simpler Bloom's levels

## License

This tool is provided as-is for educational use. Please respect Google AI API terms of service.

## Support

For issues or questions, please check the browser console for error messages and ensure your API key is properly configured.
