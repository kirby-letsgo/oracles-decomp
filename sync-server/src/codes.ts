import { randomInt } from 'node:crypto';

// 16 random digits (about 53 bits): no password, but not guessable either.
export function newCode(): string {
  let code = '';
  for (let i = 0; i < 16; i++) code += randomInt(10).toString();
  return code;
}

// "4827 1930-5561.0284" and "4827193055610284" are the same code.
export function normalizeCode(input: string): string | null {
  const digits = input.replace(/[\s.-]/g, '');
  return /^\d{16}$/.test(digits) ? digits : null;
}

export function formatCode(code: string): string {
  return code.replace(/(\d{4})(?=\d)/g, '$1-');
}
