import { WebSocket } from 'ws';
import * as wav from 'wav';
import { Writable } from 'stream';
import * as fs from 'fs';
import path from 'path';

export interface AudioConfig {
    sampleRate: number;
    channels: number;
    bitDepth: number;
}

export enum SampleRate {
    RATE_16000 = 16000,
    RATE_44100 = 44100,
    RATE_24000 = 24000,
    RATE_22050 = 22050
}

export class AudioManager {
    private configMediumDef: AudioConfig = {
        sampleRate: 24000,
        channels: 1,
        bitDepth: 16
    };

    private configLowDef: AudioConfig = {
        sampleRate: 16000,
        channels: 1,
        bitDepth: 16
    };

    private configHighDef: AudioConfig = {
        sampleRate: 44100,
        channels: 1,
        bitDepth: 32
    };

    private configUltraHighDef: AudioConfig = {
        sampleRate: 96000,
        channels: 1,
        bitDepth: 16
    };

    private config24Bit: AudioConfig = {
        sampleRate: 16000,
        channels: 1,
        bitDepth: 24
    }

    private config16Bit: AudioConfig = {
        sampleRate: 16000, // or 44100 if you must
        channels: 1,
        bitDepth: 16
      };

    private fileWriter: wav.FileWriter | undefined;
    private writeTimeout: NodeJS.Timeout | null = null;
    private isProcessing: boolean = false;
    private config = this.config16Bit;

    constructor() {
        // Don't initialize file writer in constructor
    }

    private initializeFileWriter(filename: string) {
        this.fileWriter = new wav.FileWriter(filename, {
            sampleRate: this.config.sampleRate,
            channels: this.config.channels,
            bitDepth: this.config.bitDepth
        });
    }

    public resetRecording(): void {
        if (this.writeTimeout) {
            clearTimeout(this.writeTimeout);
            this.writeTimeout = null;
        }
        
        // Close existing file writer if any
        if (this.fileWriter) {
            this.fileWriter.end();
            this.fileWriter = undefined;
        }
        
        // Reset buffer and processing state
        this.audioBuffer = Buffer.alloc(0);
        this.isProcessing = false;
    }

    public startRecording() {
        // Reset any existing recording state
        this.resetRecording();
        
        // Generate random ID for filename
        const randomId = Math.random().toString(36).substring(2, 15);
        const filename = path.join(__dirname, '../../tmp', `recording-${randomId}.wav`);
        
        // Initialize new file writer with random filename
        this.initializeFileWriter(filename);
        console.log('Started new recording:', filename);
    }

    private audioBuffer: Buffer = Buffer.alloc(0);
    private readonly WRITE_DELAY = 500; // Reduced to 500ms for more responsive writes
    private readonly MAX_BUFFER_SIZE = 1024 * 1024; // 1MB max buffer size to prevent memory issues
    private readonly MIN_BUFFER_SIZE = this.config.sampleRate; // 1 second worth of audio data

    public handleAudioBuffer(buffer: Buffer): void {
        try {
            // Check if adding new buffer would exceed max size
            if (this.audioBuffer.length + buffer.length > this.MAX_BUFFER_SIZE) {
                // Process existing buffer before adding more
                this.processAndWriteBuffer();
            }

            // Concatenate incoming buffer with existing data
            this.audioBuffer = Buffer.concat([this.audioBuffer, buffer]);

            // Reset the write timeout
            if (this.writeTimeout) {
                clearTimeout(this.writeTimeout);
            }

            // Set new timeout to trigger write
            this.writeTimeout = setTimeout(() => {
                if (this.audioBuffer.length > 0) {
                    this.processAndWriteBuffer();
                }
            }, this.WRITE_DELAY);

            // If buffer exceeds minimum size, process immediately
            if (this.audioBuffer.length >= this.MIN_BUFFER_SIZE && !this.isProcessing) {
                this.processAndWriteBuffer();
            }

        } catch (error) {
            console.error('Error handling audio buffer:', error);
            // Log more details about the error
            if (error instanceof Error) {
                console.error('Error details:', error.message, error.stack);
            }
            this.audioBuffer = Buffer.alloc(0);
            this.isProcessing = false;
        }
    }

    private processAndWriteBufferSimple(): void {
        console.log('******Processing audio buffer:', {
          bufferSize: this.audioBuffer.length,
          hasFileWriter: !!this.fileWriter,
          isProcessing: this.isProcessing
        });
      
        if (!this.fileWriter || this.audioBuffer.length === 0 || this.isProcessing) {
          console.log('Skipping write - conditions not met:', {
            hasFileWriter: !!this.fileWriter,
            bufferLength: this.audioBuffer.length,
            isProcessing: this.isProcessing
          });
          return;
        }
      
        try {
          this.isProcessing = true;
          // Take the entire buffered data
          const bufferToWrite = this.audioBuffer;
          // Clear it out
          this.audioBuffer = Buffer.alloc(0);
      
          // Convert from 32-bit (really 24 bits used) to 16-bit
          const buffer16 = this.convert32bitTo16bit(bufferToWrite);
      
          // Write 16-bit data to the WAV file
          this.fileWriter.write(buffer16);
      
          console.log(`Successfully wrote ${buffer16.length} bytes (16-bit) of audio data`);
        } catch (error) {
          console.error('Error writing audio buffer:', error);
        } finally {
          this.isProcessing = false;
        }
    }

    private convert32bitTo16bit(inBuffer: Buffer): Buffer {
        if (inBuffer.length % 4 !== 0) {
          console.warn(
            'Warning: incoming buffer length is not a multiple of 4 bytes. ' + 
            'Data may be truncated or misaligned.'
          );
        }
      
        const frameCount = Math.floor(inBuffer.length / 4);
        // Each 16-bit sample is 2 bytes
        const outBuffer = Buffer.alloc(frameCount * 2);
      
        for (let i = 0; i < frameCount; i++) {
          // Example: we assume the format is [empty, B1, B2, B3], 
          // where B1..B3 is the 24-bit signed audio
          const b1 = inBuffer[i * 4 + 1];
          const b2 = inBuffer[i * 4 + 2];
          const b3 = inBuffer[i * 4 + 3];
      
          // Combine them into a 24-bit signed value
          let sample24 = (b3 << 16) | (b2 << 8) | b1;
      
          // If the sign bit of a 24-bit sample is set (bit 23),
          // then sign-extend into the 32-bit integer:
          if (sample24 & 0x800000) {
            sample24 |= 0xFF000000;
          }
      
          // Now sample24 is a signed 32-bit integer holding a 24-bit value.
          // We reduce it to 16 bits by shifting right 8 bits:
          const sample16 = sample24 >> 8;
      
          // Write the 16-bit sample in little-endian
          outBuffer.writeInt16LE(sample16, i * 2);
        }
      
        return outBuffer;
    }
      
      
    // private processAndWriteBufferSimple(): void {
    //     console.log('******Processing audio buffer:', {
    //         bufferSize: this.audioBuffer.length,
    //         hasFileWriter: !!this.fileWriter,
    //         isProcessing: this.isProcessing
    //     });
    
    //     // Skip if we have no data or we're already processing
    //     if (!this.fileWriter || this.audioBuffer.length === 0 || this.isProcessing) {
    //         console.log('Skipping write - conditions not met');
    //         return;
    //     }
    
    //     try {
    //         this.isProcessing = true;
    //         // Grab the buffer, then clear
    //         const bufferToWrite = this.audioBuffer;
    //         this.audioBuffer = Buffer.alloc(0);
    
    //         // 1) Convert from 32-bit frames (24 bits audio + 8 “empty”) to real 24-bit
    //         const buffer24 = this.convert32bitTo24bit(bufferToWrite);
    
    //         // 2) Write it out
    //         this.fileWriter.write(buffer24);
    
    //         console.log(`Successfully wrote ${buffer24.length} bytes (24-bit) of audio data`);
    //     } catch (error) {
    //         console.error('Error writing audio buffer:', error);
    //     } finally {
    //         this.isProcessing = false;
    //     }
    // }
    
    /**
     * Converts 32-bit frames (with only 24 valid bits) into proper 24-bit samples.
     * @param inBuffer - A Buffer whose length is a multiple of 4.
     * @returns A new Buffer containing 3 bytes per sample (24-bit).
     */
    private convert32bitTo24bit(inBuffer: Buffer): Buffer {
        if (inBuffer.length % 4 !== 0) {
            console.warn('Incoming buffer is not a multiple of 4 bytes, which is unexpected for 32-bit frames!');
        }
    
        const frameCount = Math.floor(inBuffer.length / 4);
        const outBuffer = Buffer.alloc(frameCount * 3);
    
        // Example: if the empty byte is the lowest-order byte, 
        // your real data might be in inBuffer[i*4 + 1..3].
        // Adjust as needed based on actual data alignment on your ESP32.
        for (let i = 0; i < frameCount; i++) {
            // Example: skip the lowest-order byte [i*4 + 0] if that's empty
            outBuffer[i * 3 + 0] = inBuffer[i * 4 + 1]; // lower 8 bits
            outBuffer[i * 3 + 1] = inBuffer[i * 4 + 2];
            outBuffer[i * 3 + 2] = inBuffer[i * 4 + 3]; // highest 8 bits
        }
    
        return outBuffer;
    }
    
    // private processAndWriteBufferSimple(): void {
    //     // Log the current state before processing
    //     console.log('******Processing audio buffer:', {
    //         bufferSize: this.audioBuffer.length,
    //         hasFileWriter: !!this.fileWriter,
    //         isProcessing: this.isProcessing
    //     });
    //     if (!this.fileWriter || this.audioBuffer.length === 0 || this.isProcessing) {
    //         console.log('Skipping write - conditions not met:', {
    //             hasFileWriter: !!this.fileWriter,
    //             bufferLength: this.audioBuffer.length,
    //             isProcessing: this.isProcessing
    //         });
    //         return;
    //     }

    //     try {
    //         this.isProcessing = true;
    //         // Write buffer and clear in one atomic operation
    //         const bufferToWrite = this.audioBuffer;
    //         this.audioBuffer = Buffer.alloc(0);
            
    //         this.fileWriter.write(bufferToWrite);
            
    //         // Log successful write
    //         console.log(`Successfully wrote ${bufferToWrite.length} bytes of audio data`);
    //     } catch (error) {
    //         console.error('Error writing audio buffer:', error);
    //     } finally {
    //         this.isProcessing = false;
    //     }
    // }

    public getCurrentBuffer(): Buffer {
        if (!this.fileWriter) {
            console.error('No file writer available');
            return Buffer.alloc(0);
        }

        try {
            const audioFilePath = this.fileWriter.path;
            const fileBuffer = fs.readFileSync(audioFilePath);
            console.log(`Successfully read audio file: ${audioFilePath}`);
            return fileBuffer;
        } catch (error) {
            console.error('Error reading current audio buffer:', error);
            return Buffer.alloc(0);
        }
    }

    private processAndWriteBuffer(): void {
        // return this.processAndWriteBufferWithGain();
        return this.processAndWriteBufferSimple();
    }

    private processAndWriteBufferWithGain(): void {
        if (this.isProcessing || this.audioBuffer.length === 0) return;

        this.isProcessing = true;
        try {
            // Convert buffer to 16-bit PCM samples
            const samples = new Int16Array(this.audioBuffer.length / 2);
            for (let i = 0; i < this.audioBuffer.length; i += 2) {
                // Read samples directly without distortion
                const sample = this.audioBuffer.readInt16LE(i);
                samples[i / 2] = sample;
            }

            // Process audio only if there's meaningful data
            const maxAmplitude = Math.max(...Array.from(samples).map(Math.abs));
            
            if (maxAmplitude > 100) {
                const processedBuffer = this.processAudioSamples(samples, maxAmplitude);
                this.fileWriter?.write(processedBuffer);
                console.log(`Processed and wrote ${this.audioBuffer.length} bytes of audio data`);
            } else {
                console.log('Skipping buffer - insufficient audio level');
            }

            // Clear the buffer after processing
            this.audioBuffer = Buffer.alloc(0);
        } finally {
            this.isProcessing = false;
        }
    }
    private processAudioSamples(samples: Int16Array, maxAmplitude: number): Buffer {
        const processedSamples = new Int16Array(samples.length);
        const GAIN = 0.1; // Gain factor for normalization
        // Reduce normalization intensity
        const normalizeRatio = maxAmplitude > 0 ? (32767 / maxAmplitude) * GAIN : 1;
        const noiseFloor = 15; // Increased noise floor
        const maxVal = 32767 * 0.6; // Reduced maximum value to prevent clipping
        
        let prevSample = 0; // For simple low-pass filter
        const smoothingFactor = 0.1; // Adjust between 0 and 1

        for (let i = 0; i < samples.length; i++) {
            if (Math.abs(samples[i]) < noiseFloor) {
                processedSamples[i] = 0;
                continue;
            }

            let normalizedSample = samples[i] * normalizeRatio;
            
            // Apply simple low-pass filter
            normalizedSample = prevSample + smoothingFactor * (normalizedSample - prevSample);
            prevSample = normalizedSample;

            processedSamples[i] = Math.round(
                Math.max(Math.min(normalizedSample, maxVal), -maxVal)
            );
        }

        return Buffer.from(processedSamples.buffer);
    }

    public closeFile(): void {
        console.log('Closing WAV file writer');
        if (this.writeTimeout) {
            clearTimeout(this.writeTimeout);
        }
        // Process any remaining audio data
        if (this.audioBuffer.length > 0) {
            this.processAndWriteBuffer();
        }
        // if (this.fileWriter) {
        //     this.fileWriter.end();
        //     this.fileWriter = undefined; // Clear the reference
        // }
    }
}
