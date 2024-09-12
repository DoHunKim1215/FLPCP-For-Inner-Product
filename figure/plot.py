import math

import numpy as np
from matplotlib import pyplot as plt

class FLPCP:
    xs = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [88, 152, 280, 536, 1048, 2072, 4120, 8216, 16408, 32792, 65560]
        predictions = [16 * x + 24 for x in FLPCP.xs]

        ax.plot(FLPCP.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLPCP.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]

        ax.plot(FLPCP.xs, ys, color='red', marker='o')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.yticks(ticks=[4], labels=['4'])
        plt.tight_layout()
        fig.savefig(f'query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.009600, 0.015400000, 0.067500000, 0.426100000, 3.111200000, 24.086200000, 184.460500000, 1450.466200000, 12067.545000000, 92834.565900000, 741167.230300000]
        poly_a = np.polyfit(FLPCP.xs[0:2] + FLPCP.xs[5:7], ys[0:2] + ys[5:7], 3)
        predictions = [poly_a[0] * x * x * x + poly_a[1] * x * x + poly_a[2] * x + poly_a[3] for x in FLPCP.xs]

        ys_precompute = [0.001700000, 0.002100000, 0.005200000, 0.013800000, 0.050000000, 0.189400000, 0.735900000, 2.908500000, 11.614400000, 46.401400000, 189.302900000]
        poly_b = np.polyfit(FLPCP.xs[0:2] + FLPCP.xs[6:7], ys_precompute[0:2] + ys_precompute[6:7], 2)
        predictions_precompute = [poly_b[0] * x * x + poly_b[1] * x + poly_b[2] for x in FLPCP.xs]

        ax.plot(FLPCP.xs, ys, color='red', marker='o', label='not precomputed')
        ax.plot(FLPCP.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^3$')
        ax.plot(FLPCP.xs, ys_precompute, color='green', marker='s', label='precomputed')
        ax.plot(FLPCP.xs, predictions_precompute, color='magenta', marker='v', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.010600000, 0.025200000, 0.091500000, 0.344100000, 1.373100000, 5.306800000, 21.071200000, 83.892500000, 334.685600000, 1338.557400000, 5363.491900000]
        poly_a = np.polyfit(FLPCP.xs[0:1] + FLPCP.xs[5:7], ys[0:1] + ys[5:7], 2)
        predictions = [poly_a[0] * x * x + poly_a[1] * x + poly_a[2] for x in FLPCP.xs]

        ax.plot(FLPCP.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCP.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCP.xs, labels=FLPCP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLPCP.proof_size()
        FLPCP.query_complexity()
        FLPCP.prover_time()
        FLPCP.verifier_time()


class FLPCPSqrt:
    xs = [4, 16, 64, 256, 1024, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13, 2)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [72, 136, 264, 520, 1032, 2056]
        predictions = [32 * math.sqrt(x) + 8 for x in FLPCPSqrt.xs]

        ax.plot(FLPCPSqrt.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLPCPSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPSqrt.xs, labels=FLPCPSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [6, 10, 18, 34, 66, 130]
        predictions = [2 * math.sqrt(x) + 2 for x in FLPCPSqrt.xs]

        ax.plot(FLPCPSqrt.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPSqrt.xs, labels=FLPCPSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.007800000, 0.028900000, 0.118600000, 1.071100000, 13.440900000, 194.183000000]
        poly = np.polyfit(FLPCPSqrt.xs[0:1] + FLPCPSqrt.xs[3:4] + FLPCPSqrt.xs[5:6], ys[0:1] + ys[3:4] + ys[5:6], 2)
        predictions = [poly[0] * x * x + poly[1] * x + poly[2] for x in FLPCPSqrt.xs]

        precomputed = [0.001900000, 0.003400000, 0.010400000, 0.063000000, 0.412400000, 3.087300000]
        poly_b = np.polyfit([math.sqrt(x) for x in [4, 16, 256, 1024]],
                          [0.001900000, 0.003400000, 0.063000000, 0.412400000], 3)
        predictions_precomputed = [poly_b[0] * x * math.sqrt(x) + poly_b[1] * x + poly_b[2] * math.sqrt(x) + poly_b[3] for x in
                       FLPCPSqrt.xs]

        ax.plot(FLPCPSqrt.xs, ys, color='red', marker='o', label='not precomputed')
        ax.plot(FLPCPSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')
        ax.plot(FLPCPSqrt.xs, precomputed, color='green', marker='s', label='precomputed')
        ax.plot(FLPCPSqrt.xs, predictions_precomputed, color='magenta', marker='v', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPSqrt.xs, labels=FLPCPSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.004900000, 0.017000000, 0.061300000, 0.324700000, 1.892400000, 13.300400000]
        poly = np.polyfit([math.sqrt(x) for x in [4, 16, 256, 1024]], [0.004900000, 0.017000000, 0.324700000, 1.892400000], 3)
        predictions = [poly[0] * x * math.sqrt(x) + poly[1] * x + poly[2] * math.sqrt(x) + poly[3] for x in FLPCPSqrt.xs]

        ax.plot(FLPCPSqrt.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPSqrt.xs, labels=FLPCPSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLPCPSqrt.proof_size()
        FLPCPSqrt.query_complexity()
        FLPCPSqrt.prover_time()
        FLPCPSqrt.verifier_time()


class FLPCPCoeff:
    xs = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [88, 152, 280, 536, 1048, 2072, 4120, 8216, 16408, 32792, 65560]
        predictions = [16 * x + 24 for x in FLPCPCoeff.xs]

        ax.plot(FLPCPCoeff.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeff.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeff.xs, labels=FLPCPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4]

        ax.plot(FLPCPCoeff.xs, ys, color='red', marker='o')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.xticks(ticks=FLPCPCoeff.xs, labels=FLPCPCoeff.x_labels)
        plt.yticks(ticks=[4], labels=['4'])
        plt.tight_layout()
        fig.savefig(f'query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.001400000, 0.001200000, 0.001800000, 0.004400000, 0.015100000, 0.056900000, 0.220500000, 0.865000000, 3.468100000, 13.808900000, 54.619200000]
        poly = np.polyfit(FLPCPCoeff.xs[1:3] + FLPCPCoeff.xs[5:6], ys[1:3] + ys[5:6], 2)
        predictions = [poly[0] * x * x + poly[1] * x + poly[2] for x in FLPCPCoeff.xs]

        ax.plot(FLPCPCoeff.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeff.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeff.xs, labels=FLPCPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.002300000, 0.001900000, 0.005900000, 0.007100000, 0.011500000, 0.022700000, 0.046000000, 0.090700000, 0.186000000, 0.373700000, 0.755800000]
        poly = np.polyfit(FLPCPCoeff.xs[1:2] + FLPCPCoeff.xs[7:8], ys[1:2] + ys[7:8], 1)
        predictions = [poly[0] * x + poly[1] for x in FLPCPCoeff.xs]

        ax.plot(FLPCPCoeff.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeff.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeff.xs, labels=FLPCPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLPCPCoeff.proof_size()
        FLPCPCoeff.query_complexity()
        FLPCPCoeff.prover_time()
        FLPCPCoeff.verifier_time()


class FLPCPCoeffSqrt:
    xs = [4, 16, 64, 256, 1024, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13, 2)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [72, 136, 264, 520, 1032, 2056]
        predictions = [32 * math.sqrt(x) + 8 for x in FLPCPCoeffSqrt.xs]

        ax.plot(FLPCPCoeffSqrt.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeffSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [6, 10, 18, 34, 66, 130]
        predictions = [2 * math.sqrt(x) + 2 for x in FLPCPCoeffSqrt.xs]

        ax.plot(FLPCPCoeffSqrt.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeffSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig('query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.001400000, 0.002700000, 0.004600000, 0.021000000, 0.131000000, 0.961800000]
        poly = np.polyfit([math.sqrt(x) for x in FLPCPCoeffSqrt.xs[1:4] + FLPCPCoeffSqrt.xs[5:6]], ys[1:4] + ys[5:6], 3)
        predictions = [poly[0] * x * math.sqrt(x) + poly[1] * x + poly[2] * math.sqrt(x) + poly[3] for x in FLPCPCoeffSqrt.xs]

        ax.plot(FLPCPCoeffSqrt.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeffSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.002000000, 0.008800000, 0.026900000, 0.189700000, 1.311300000, 10.675500000]
        poly = np.polyfit([math.sqrt(x) for x in FLPCPCoeffSqrt.xs[0:2] + FLPCPCoeffSqrt.xs[4:6]], ys[0:2] + ys[4:6], 3)
        predictions = [poly[0] * x * math.sqrt(x) + poly[1] * x + poly[2] * math.sqrt(x) + poly[3] for x in
                       FLPCPCoeffSqrt.xs]

        ax.plot(FLPCPCoeffSqrt.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLPCPCoeffSqrt.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLPCPCoeffSqrt.proof_size()
        FLPCPCoeffSqrt.query_complexity()
        FLPCPCoeffSqrt.prover_time()
        FLPCPCoeffSqrt.verifier_time()


class ComparisonFLPCPMethods:
    xs = [4, 16, 64, 256, 1024, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13, 2)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        flpcp = [88, 280, 1048, 4120, 16408, 65560]
        flpcp_sqrt = [72, 136, 264, 520, 1032, 2056]
        flpcp_coeff = [88, 280, 1048, 4120, 16408, 65560]
        flpcp_coeff_sqrt = [72, 136, 264, 520, 1032, 2056]

        ax.plot(FLPCPCoeffSqrt.xs, flpcp, color='red', marker='o', label='Baseline')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff, color='blue', marker='^', label='Coefficient')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_sqrt, color='green', marker='s', label='Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff_sqrt, color='magenta', marker='v', label='Coefficient + Sqrt')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        flpcp = [4, 4, 4, 4, 4, 4]
        flpcp_sqrt = [6, 10, 18, 34, 66, 130]
        flpcp_coeff = [4, 4, 4, 4, 4, 4]
        flpcp_coeff_sqrt = [6, 10, 18, 34, 66, 130]

        ax.plot(FLPCPCoeffSqrt.xs, flpcp_sqrt, color='green', marker='s', label='Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff_sqrt, color='magenta', marker='v', label='Coefficient + Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp, color='red', marker='o', label='Baseline')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff, color='blue', marker='^', label='Coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig('query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        flpcp = [0.009600, 0.067500000, 3.111200000, 184.460500000, 12067.545000000, 741167.230300000]
        flpcp_sqrt = [0.007800000, 0.028900000, 0.118600000, 1.071100000, 13.440900000, 194.183000000]
        flpcp_precompute = [0.001700000, 0.005200000, 0.050000000, 0.735900000, 11.614400000, 189.302900000]
        flpcp_coeff = [0.001400000, 0.001800000, 0.015100000, 0.220500000, 3.468100000, 54.619200000]
        flpcp_sqrt_precomputed = [0.001900000, 0.003400000, 0.010400000, 0.063000000, 0.412400000, 3.087300000]
        flpcp_coeff_sqrt = [0.001400000, 0.002700000, 0.004600000, 0.021000000, 0.131000000, 0.961800000]

        ax.plot(FLPCPCoeffSqrt.xs, flpcp, color='red', marker='o', label='Baseline')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_sqrt, color='green', marker='s', label='Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_precompute, color='orange', marker='d', label='Baseline (precomputed)')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff, color='blue', marker='^', label='Coefficient')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_sqrt_precomputed, color='purple', marker='*', label='Sqrt (precomputed)')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff_sqrt, color='magenta', marker='v', label='Coefficient + Sqrt')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        flpcp = [0.010600000, 0.091500000, 1.373100000, 21.071200000, 334.685600000, 5363.491900000]
        flpcp_sqrt = [0.004900000, 0.017000000, 0.061300000, 0.324700000, 1.892400000, 13.300400000]
        flpcp_coeff = [0.002300000, 0.005900000, 0.011500000, 0.046000000, 0.186000000, 0.755800000]
        flpcp_coeff_sqrt = [0.002000000, 0.008800000, 0.026900000, 0.189700000, 1.311300000, 10.675500000]

        ax.plot(FLPCPCoeffSqrt.xs, flpcp, color='red', marker='o', label='Baseline')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_sqrt, color='green', marker='s', label='Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff_sqrt, color='magenta', marker='v', label='Coefficient + Sqrt')
        ax.plot(FLPCPCoeffSqrt.xs, flpcp_coeff, color='blue', marker='^', label='Coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLPCPCoeffSqrt.xs, labels=FLPCPCoeffSqrt.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        ComparisonFLPCPMethods.proof_size()
        ComparisonFLPCPMethods.query_complexity()
        ComparisonFLPCPMethods.prover_time()
        ComparisonFLPCPMethods.verifier_time()


class FLIOP:
    xs = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [80, 104, 128, 152, 176, 200, 224, 248, 272, 296, 320]
        predictions = [24 * math.log(x, 2) + 32 for x in FLIOP.xs]

        ax.plot(FLIOP.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLIOP.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26]
        predictions = [2 * math.log(x, 2) + 2 for x in FLIOP.xs]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLIOP.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.015600, 0.013700000, 0.027000000, 0.053300000, 0.164300000, 0.220100000, 0.427100000, 0.920300000, 1.789600000, 3.513800000, 7.552700000]
        ys_oracle = [0.019400000, 0.015000000, 0.029100000, 0.057100000, 0.162300000, 0.234200000, 0.447500000, 0.884400000, 1.824900000, 3.587200000, 7.229200000]
        poly = np.polyfit(FLIOP.xs, ys_oracle, 1)
        predictions_oracle = [poly[0] * x + poly[1] for x in FLIOP.xs]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_oracle, color='blue', marker='^', label='with random oracle')
        ax.plot(FLIOP.xs, predictions_oracle, color='green', marker='s', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.005900000, 0.003300000, 0.003100000, 0.003400000, 0.003800000, 0.003700000, 0.004800000, 0.005000000, 0.005000000, 0.006100000, 0.008700000]
        ys_oracle = [0.004800000, 0.003900000, 0.004000000, 0.004500000, 0.004800000, 0.005100000, 0.005600000, 0.006100000, 0.006900000, 0.007400000, 0.008000000]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel('Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.105600000, 0.169760000, 0.233920000, 0.298080000, 0.362240000, 0.426400000, 0.490560000, 0.554720000, 0.618880000, 0.683040000, 0.747200000]
        ys_oracle = [0.046640000, 0.050800000, 0.054960000, 0.059120000, 0.063280000, 0.067440000, 0.071600000, 0.075760000, 0.079920000, 0.084080000, 0.088240000]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Communication delay in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'lan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [140.400000000, 225.840000000, 311.280000000, 396.720000000, 482.160000000, 567.600000000, 653.040000000, 738.480000000, 823.920000000, 909.360000000, 994.800000000]
        ys_oracle = [61.760000000, 67.200000000, 72.640000000, 78.080000000, 83.520000000, 88.960000000, 94.400000000, 99.840000000, 105.280000000, 110.720000000, 116.160000000]

        ax.plot(FLIOP.xs, ys, color='red', marker='o', label='baseline')
        ax.plot(FLIOP.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel('Input vector length')
        plt.ylabel('Communication delay in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOP.xs, labels=FLIOP.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOP.proof_size()
        FLIOP.query_complexity()
        FLIOP.prover_time()
        FLIOP.verifier_time()
        FLIOP.lan_time()
        FLIOP.wan_time()


class FLIOPCoeff:
    xs = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13)]

    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        proof_lengths = [80, 104, 128, 152, 176, 200, 224, 248, 272, 296, 320]
        predictions = [24 * math.log(x, 2) + 32 for x in FLIOPCoeff.xs]

        ax.plot(FLIOPCoeff.xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(FLIOPCoeff.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'proof_size.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26]
        predictions = [2 * math.log(x, 2) + 2 for x in FLIOPCoeff.xs]

        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='measurement')
        ax.plot(FLIOPCoeff.xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'query_complexity.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.006700, 0.002900000, 0.004900000, 0.010800000, 0.023100000, 0.042000000, 0.080400000, 0.171600000, 0.408100000, 0.695500000, 1.336800000]
        ys_oracle = [0.008300000, 0.004500000, 0.007500000, 0.015300000, 0.026700000, 0.052500000, 0.096500000, 0.190000000, 0.379600000, 0.774000000, 1.579300000]
        poly = np.polyfit(FLIOPCoeff.xs, ys_oracle, 1)
        predictions_oracle = [poly[0] * x + poly[1] for x in FLIOPCoeff.xs]

        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='coefficient')
        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='with random oracle')
        ax.plot(FLIOPCoeff.xs, predictions_oracle, color='green', marker='s', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.004100000, 0.001300000, 0.001400000, 0.001400000, 0.001300000, 0.001500000, 0.002000000, 0.002600000, 0.003100000, 0.003500000, 0.003200000]
        ys_oracle = [0.003300000, 0.001800000, 0.002100000, 0.002500000, 0.002700000, 0.003000000, 0.003500000, 0.003800000, 0.004200000, 0.004600000, 0.005400000]

        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='coefficient')
        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel('Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def lan_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.105600000, 0.169760000, 0.233920000, 0.298080000, 0.362240000, 0.426400000, 0.490560000, 0.554720000, 0.618880000, 0.683040000, 0.747200000]
        ys_oracle = [0.046640000, 0.050800000, 0.054960000, 0.059120000, 0.063280000, 0.067440000, 0.071600000, 0.075760000, 0.079920000, 0.084080000, 0.088240000]

        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='coefficient')
        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Communication delay in LAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'lan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def wan_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [140.400000000, 225.840000000, 311.280000000, 396.720000000, 482.160000000, 567.600000000, 653.040000000, 738.480000000, 823.920000000, 909.360000000, 994.800000000]
        ys_oracle = [61.760000000, 67.200000000, 72.640000000, 78.080000000, 83.520000000, 88.960000000, 94.400000000, 99.840000000, 105.280000000, 110.720000000, 116.160000000]

        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='coefficient')
        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='with random oracle')

        plt.xlabel('Input vector length')
        plt.ylabel('Communication delay in WAN (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'wan_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        FLIOPCoeff.proof_size()
        FLIOPCoeff.query_complexity()
        FLIOPCoeff.prover_time()
        FLIOPCoeff.verifier_time()
        FLIOPCoeff.lan_time()
        FLIOPCoeff.wan_time()


class ComparisonFLIOPMethods:
    xs = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
    x_labels = [fr'$2^{{{x}}}$' for x in range(2, 13)]

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.015600, 0.013700000, 0.027000000, 0.053300000, 0.164300000, 0.220100000, 0.427100000, 0.920300000,
              1.789600000, 3.513800000, 7.552700000]
        ys_oracle = [0.019400000, 0.015000000, 0.029100000, 0.057100000, 0.162300000, 0.234200000, 0.447500000,
                     0.884400000, 1.824900000, 3.587200000, 7.229200000]
        coeff = [0.006700, 0.002900000, 0.004900000, 0.010800000, 0.023100000, 0.042000000, 0.080400000, 0.171600000, 0.408100000, 0.695500000, 1.336800000]
        coeff_oracle = [0.008300000, 0.004500000, 0.007500000, 0.015300000, 0.026700000, 0.052500000, 0.096500000, 0.190000000, 0.379600000, 0.774000000, 1.579300000]

        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='Baseline + Random oracle')
        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='Baseline')
        ax.plot(FLIOPCoeff.xs, coeff_oracle, color='magenta', marker='v', label='Coefficient + Random oracle')
        ax.plot(FLIOPCoeff.xs, coeff, color='green', marker='s', label='Coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.yscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'prover_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        ys = [0.005900000, 0.003300000, 0.003100000, 0.003400000, 0.003800000, 0.003700000, 0.004800000, 0.005000000,
              0.005000000, 0.006100000, 0.008700000]
        ys_oracle = [0.004800000, 0.003900000, 0.004000000, 0.004500000, 0.004800000, 0.005100000, 0.005600000,
                     0.006100000, 0.006900000, 0.007400000, 0.008000000]
        coeff = [0.004100000, 0.001300000, 0.001400000, 0.001400000, 0.001300000, 0.001500000, 0.002000000, 0.002600000, 0.003100000, 0.003500000, 0.003200000]
        coeff_oracle = [0.003300000, 0.001800000, 0.002100000, 0.002500000, 0.002700000, 0.003000000, 0.003500000, 0.003800000, 0.004200000, 0.004600000, 0.005400000]

        ax.plot(FLIOPCoeff.xs, ys_oracle, color='blue', marker='^', label='Baseline + Random oracle')
        ax.plot(FLIOPCoeff.xs, ys, color='red', marker='o', label='Baseline')
        ax.plot(FLIOPCoeff.xs, coeff_oracle, color='magenta', marker='v', label='Coefficient + Random oracle')
        ax.plot(FLIOPCoeff.xs, coeff, color='green', marker='s', label='Coefficient')

        plt.xlabel('Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=FLIOPCoeff.xs, labels=FLIOPCoeff.x_labels)
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'verifier_time.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def plot():
        ComparisonFLIOPMethods.prover_time()
        ComparisonFLIOPMethods.verifier_time()


def wan():
    fig, ax = plt.subplots(figsize=(5, 4))

    xs = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]

    y0 = [210.210900000, 455.654700000, 701.100500000, 946.551100000, 1192.010300000, 1437.487300000, 1682.999500000, 1928.590700000, 2174.326900000, 2420.351300000]
    y1 = [210.205900000, 455.647800000, 701.091600000, 946.535700000, 1191.981700000, 1437.433200000, 1682.893000000, 1928.371200000, 2173.889200000, 2419.495200000]
    y2 = [210.210900000, 455.654700000, 458.377700000, 463.833700000, 474.817300000, 497.361600000, 717.598500000, 727.460600000, 741.247800000, 764.176700000]
    y3 = [210.205900000, 455.647800000, 458.367800000, 463.808600000, 474.691000000, 496.456000000, 717.419600000, 726.950800000, 739.213800000, 758.304400000]

    ax.plot(xs, y0, color='red', marker='o', label='FLIOP')
    ax.plot(xs, y1, color='blue', marker='v', label='FLIOP Coeff.')
    ax.plot(xs, y2, color='green', marker='s', label='FLIOP Optimum')
    ax.plot(xs, y3, color='magenta', marker='^', label='FLIOP Coeff. Optimum')

    plt.xlabel('Input vector length')
    plt.ylabel('Total time (ms)')
    plt.xscale('log')
    plt.xticks(ticks=xs, labels=[r'$2^1$', r'$2^2$', r'$2^3$', r'$2^4$', r'$2^5$', r'$2^6$', r'$2^7$', r'$2^8$', r'$2^9$', r'$2^{10}$'])
    plt.legend()
    plt.tight_layout()
    fig.savefig(f'FLIOP_WAN.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
    plt.show()


def lan():
    fig, ax = plt.subplots(figsize=(5, 4))

    xs = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]

    y0 = [0.168700000, 0.356660000, 0.546620000, 0.741380000, 0.944740000, 1.165900000, 1.422260000, 1.757620000, 2.237980000, 3.006540000]
    y2 = [0.163700000, 0.349760000, 0.537720000, 0.725980000, 0.916140000, 1.111800000, 1.315760000, 1.538120000,
          1.800280000, 2.150440000]
    y1 = [0.168700000, 0.356660000, 0.361740000, 0.381900000, 0.493820000, 0.610940000, 0.718940000, 0.964380000, 1.237780000, 1.814020000]
    y3 = [0.163700000, 0.349760000, 0.351840000, 0.356800000, 0.367520000, 0.389160000, 0.558200000, 0.576680000, 0.609040000, 0.674200000]

    ax.plot(xs, y0, color='red', marker='o', label='FLIOP')
    ax.plot(xs, y2, color='blue', marker='v', label='FLIOP Coeff.')
    ax.plot(xs, y1, color='green', marker='s', label='FLIOP Optimum')
    ax.plot(xs, y3, color='magenta', marker='^', label='FLIOP Coeff. Optimum')

    plt.xlabel('Input vector length')
    plt.ylabel('Total time (ms)')
    plt.xscale('log')
    plt.xticks(ticks=xs, labels=[r'$2^1$', r'$2^2$', r'$2^3$', r'$2^4$', r'$2^5$', r'$2^6$', r'$2^7$', r'$2^8$', r'$2^9$', r'$2^{10}$'])
    plt.legend()
    plt.tight_layout()
    fig.savefig(f'FLIOP_LAN.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
    plt.show()


if __name__ == '__main__':
    ComparisonFLIOPMethods.plot()

def a():
    FLPCP.plot()
    FLPCPCoeff.plot()
    FLPCPSqrt.plot()
    FLPCPCoeffSqrt.plot()
    ComparisonFLPCPMethods.plot()
    FLIOP.plot()
    FLIOPCoeff.plot()
    ComparisonFLIOPMethods.plot()

    # Network Simulation
    lan()
    wan()
