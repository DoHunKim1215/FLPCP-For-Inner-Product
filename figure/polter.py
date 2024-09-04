import math

import numpy as np
from matplotlib import pyplot as plt

class FLPCP:
    @staticmethod
    def flpcp_proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        proof_lengths = [92, 332, 732, 1292, 2012, 2892, 3932, 5132, 6492, 8012]
        predictions = [8 * x + 12 for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Proof size (bytes)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_PROOF_SIZE_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4]

        ax.plot(xs, ys, color='red', marker='o')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Query complexity')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.yticks(ticks=[4], labels=['4'])
        plt.tight_layout()
        fig.savefig(f'FLPCP_QUERY_COMPLEXITY_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [0.021200, 0.649100000, 6.876100000, 37.580900000, 141.064100000, 418.446700000, 1050.498000000, 2333.715100000, 4724.683200000, 9061.323000000]
        predictions = [0.000009061 * math.pow(x, 3) for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^3$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Prover time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_PROVER_TIME_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [0.015600000, 0.193700000, 0.937400000, 2.938900000, 7.133200000, 14.717300000, 27.212000000, 46.365500000, 74.206300000, 114.775100000]
        predictions = [0.000114 * x * x for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Verifier time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_VERIFIER_TIME_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


class FLPCPSqrt:
    @staticmethod
    def flpcp_proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        proof_lengths = [164, 324, 484, 644, 804, 964, 1124, 1284, 1444, 1604]
        predictions = [16 * math.sqrt(x) for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Proof size (bytes)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_PROOF_SIZE.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [22, 42, 62, 82, 102, 122, 142, 162, 182, 202]
        predictions = [2 * math.sqrt(x) + 2 for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Query complexity')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_QUERY_COMPLEXITY.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [0.169000, 1.912700000, 8.778000000, 26.755900000, 63.310500000, 129.074500000, 237.618400000, 404.773000000, 642.781400000, 973.005100000]
        predictions = [0.1 * math.pow(x / 100, 2) for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Prover time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_PROVER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [0.065000000, 0.379100000, 1.144500000, 2.516100000, 4.708000000, 8.054800000, 12.766600000, 18.394200000, 26.286500000, 35.643100000]
        predictions = [0.000036 * x * math.sqrt(x) for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Verifier time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_VERIFIER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


class FLPCPCoeff:
    @staticmethod
    def flpcp_proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        proof_lengths = [92, 332, 732, 1292, 2012, 2892, 3932, 5132, 6492, 8012]
        predictions = [8 * x + 12 for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Proof size (bytes)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_PROOF_SIZE_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [4, 4, 4, 4, 4, 4, 4, 4, 4, 4]

        ax.plot(xs, ys, color='red', marker='o')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Query complexity')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.yticks(ticks=[4], labels=['4'])
        plt.tight_layout()
        fig.savefig(f'FLPCP_COEFF_QUERY_COMPLEXITY_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [0.005300, 0.005300000, 0.021900000, 0.063900000, 0.152100000, 0.311000000, 0.574600000, 0.983100000, 1.557400000, 2.403000000]
        predictions = [0.000002403 * x * x for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN^2$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Prover time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_PROVER_TIME_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        ys = [0.003000000, 0.008400000, 0.013600000, 0.024200000, 0.032100000, 0.043200000, 0.064700000, 0.089200000, 0.110500000, 0.139200000]
        predictions = [0.0001392 * x for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Verifier time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_VERIFIER_TIME_ORG.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        coeff = [0.005300, 0.005300000, 0.021900000, 0.063900000, 0.152100000, 0.311000000, 0.574600000, 0.983100000,
              1.557400000, 2.403000000]
        non_coeff = [0.021200, 0.649100000, 6.876100000, 37.580900000, 141.064100000, 418.446700000, 1050.498000000, 2333.715100000, 4724.683200000, 9061.323000000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLPCP')
        ax.plot(xs, coeff, color='blue', marker='^', label='FLPCP Coefficient')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Prover time (ms)')
        plt.yscale('log')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_PROVER_TIME_FLPCP.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        coeff = [0.003000000, 0.008400000, 0.013600000, 0.024200000, 0.032100000, 0.043200000, 0.064700000, 0.089200000,
              0.110500000, 0.139200000]
        non_coeff = [0.015600000, 0.193700000, 0.937400000, 2.938900000, 7.133200000, 14.717300000, 27.212000000, 46.365500000, 74.206300000, 114.775100000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLPCP')
        ax.plot(xs, coeff, color='blue', marker='^', label=r'FLPCP Coefficient')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Verifier time (ms)')
        plt.yscale('log')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_VERIFIER_TIME_FLPCP.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


class FLPCPCoeffSqrt:
    @staticmethod
    def flpcp_coeff_proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        proof_lengths = [164, 324, 484, 644, 804, 964, 1124, 1284, 1444, 1604]
        predictions = [16 * math.sqrt(x) for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Proof size (bytes)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_PROOF_SIZE.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_coeff_query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [22, 42, 62, 82, 102, 122, 142, 162, 182, 202]
        predictions = [2 * math.sqrt(x) + 2 for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Query complexity')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_QUERY_COMPLEXITY.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_coeff_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [0.009800, 0.030800000, 0.090200000, 0.201900000, 0.420700000, 0.580300000, 1.201000000, 1.380100000, 1.899100000, 2.596800000]
        predictions = [0.0000026 * x * math.sqrt(x) for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Prover time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_PROVER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def flpcp_coeff_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [100, 400, 900, 1600, 2500, 3600, 4900, 6400, 8100, 10000]
        ys = [0.046800000, 0.276400000, 0.858600000, 1.929400000, 3.790800000, 6.883700000, 10.330900000, 15.036700000, 20.664200000, 28.498200000]
        predictions = [0.0000285 * x * math.sqrt(x) for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN\sqrt{N}$')

        plt.xlabel(r'Input vector length ($\times 10^2$)')
        plt.ylabel('Verifier time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 100)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLPCP_COEFF_VERIFIER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        coeff = [0.009800, 0.030800000, 0.090200000, 0.201900000, 0.420700000, 0.580300000, 1.201000000, 1.380100000, 1.899100000, 2.596800000]
        non_coeff = [0.169000, 1.912700000, 8.778000000, 26.755900000, 63.310500000, 129.074500000, 237.618400000, 404.773000000, 642.781400000, 973.005100000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLPCP')
        ax.plot(xs, coeff, color='blue', marker='^', label='FLPCP Coefficient')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Prover time (ms)')
        plt.yscale('log')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_PROVER_TIME_FLPCP_SQRT.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [10, 40, 90, 160, 250, 360, 490, 640, 810, 1000]
        coeff = [0.046800000, 0.276400000, 0.858600000, 1.929400000, 3.790800000, 6.883700000, 10.330900000, 15.036700000, 20.664200000, 28.498200000]
        non_coeff = [0.065000000, 0.379100000, 1.144500000, 2.516100000, 4.708000000, 8.054800000, 12.766600000, 18.394200000, 26.286500000, 35.643100000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLPCP')
        ax.plot(xs, coeff, color='blue', marker='^', label=r'FLPCP Coefficient')

        plt.xlabel(r'Input vector length ($\times 10$)')
        plt.ylabel('Verifier time (ms)')
        plt.xticks(ticks=xs, labels=[str(int(x / 10)) for x in xs])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_VERIFIER_TIME_FLPCP_SQRT.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


class FLIOP:
    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        proof_lengths = [40, 64, 88, 112, 136, 160, 184, 208, 232, 256]
        predictions = [12 * math.log(x, 2) + 16 for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$', r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_PROOF_SIZE.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [6, 10, 14, 18, 22, 26, 30, 34, 38, 42]
        predictions = [2 * math.log(x, 2) + 2 for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_QUERY_COMPLEXITY.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [0.007100, 0.016700000, 0.054200000, 0.223900000, 0.863800000, 3.443600000, 13.545900000, 54.558700000, 219.712600000, 878.400700000]
        predictions = [(878.4007 / 1048576) * x for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_PROVER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [0.002200000, 0.002300000, 0.002200000, 0.002600000, 0.003600000, 0.004500000, 0.005900000, 0.009600000, 0.011700000, 0.014400000]
        poly = np.polyfit([math.pow(x, 1. / 4) for x in xs], ys, 1)
        predictions = [poly[0] * math.pow(x, 1. / 4) + poly[1] for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt[4]{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.legend()
        plt.tight_layout()
        fig.savefig(f'FLIOP_VERIFIER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()


class FLIOPCoeff:
    @staticmethod
    def proof_size():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        proof_lengths = [40, 64, 88, 112, 136, 160, 184, 208, 232, 256]
        predictions = [12 * math.log(x, 2) + 16 for x in xs]

        ax.plot(xs, proof_lengths, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Proof size (bytes)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_COEFF_PROOF_SIZE.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def query_complexity():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [6, 10, 14, 18, 22, 26, 30, 34, 38, 42]
        predictions = [2 * math.log(x, 2) + 2 for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\log{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Query complexity')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_COEFF_QUERY_COMPLEXITY.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [0.004000, 0.006500000, 0.021900000, 0.086300000, 0.309100000, 1.259800000, 4.689200000, 19.337500000, 76.263500000, 311.134800000]
        poly = np.polyfit(xs, ys, 1)
        predictions = [poly[0] * x + poly[1] for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$cN$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_COEFF_PROVER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        ys = [0.001500000, 0.001300000, 0.001200000, 0.001600000, 0.002000000, 0.002900000, 0.002800000, 0.006400000, 0.007300000, 0.009800000]
        poly = np.polyfit([math.pow(x, 1. / 4) for x in xs], ys, 1)
        predictions = [poly[0] * math.pow(x, 1. / 4) + poly[1] for x in xs]

        ax.plot(xs, ys, color='red', marker='o', label='measurement')
        ax.plot(xs, predictions, color='blue', marker='^', linestyle='--', label=r'$c\sqrt[4]{N}$')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'FLIOP_COEFF_VERIFIER_TIME.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_prover_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        coeff = [0.004000, 0.006500000, 0.021900000, 0.086300000, 0.309100000, 1.259800000, 4.689200000, 19.337500000,
              76.263500000, 311.134800000]
        non_coeff = [0.007100, 0.016700000, 0.054200000, 0.223900000, 0.863800000, 3.443600000, 13.545900000, 54.558700000,
              219.712600000, 878.400700000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLIOP')
        ax.plot(xs, coeff, color='blue', marker='^', label='FLIOP Coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Prover time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_PROVER_TIME_FLIOP.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

    @staticmethod
    def compare_verifier_time():
        fig, ax = plt.subplots(figsize=(5, 4))

        xs = [4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
        coeff = [0.001500000, 0.001300000, 0.001200000, 0.001600000, 0.002000000, 0.002900000, 0.002800000, 0.006400000,
              0.007300000, 0.009800000]
        non_coeff = [0.002200000, 0.002300000, 0.002200000, 0.002600000, 0.003600000, 0.004500000, 0.005900000, 0.009600000,
              0.011700000, 0.014400000]

        ax.plot(xs, non_coeff, color='red', marker='o', label='FLIOP')
        ax.plot(xs, coeff, color='blue', marker='^', label='FLIOP Coefficient')

        plt.xlabel(r'Input vector length')
        plt.ylabel('Verifier time (ms)')
        plt.xscale('log')
        plt.xticks(ticks=xs, labels=[r'$4^{1}$', r'$4^{2}$', r'$4^{3}$', r'$4^{4}$', r'$4^{5}$', r'$4^{6}$', r'$4^{7}$',
                                     r'$4^{8}$', r'$4^{9}$', r'$4^{10}$'])
        plt.tight_layout()
        plt.legend()
        fig.savefig(f'COMPARE_VERIFIER_TIME_FLIOP.png', dpi=300, format='png', bbox_inches='tight', pad_inches=0.1, )
        plt.show()

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
    # FLPCP
    FLPCP.flpcp_proof_size()
    FLPCP.flpcp_query_complexity()
    FLPCP.flpcp_prover_time()
    FLPCP.flpcp_verifier_time()

    # FLPCPCoeff
    FLPCPCoeff.flpcp_proof_size()
    FLPCPCoeff.flpcp_query_complexity()
    FLPCPCoeff.flpcp_prover_time()
    FLPCPCoeff.flpcp_verifier_time()

    # FLPCP vs FLPCP Coeff.
    FLPCPCoeff.compare_prover_time()
    FLPCPCoeff.compare_verifier_time()

    # FLPCP Sqrt.
    FLPCPSqrt.flpcp_proof_size()
    FLPCPSqrt.flpcp_query_complexity()
    FLPCPSqrt.flpcp_prover_time()
    FLPCPSqrt.flpcp_verifier_time()

    # FLPCPCoeff Sqrt.
    FLPCPCoeffSqrt.flpcp_coeff_proof_size()
    FLPCPCoeffSqrt.flpcp_coeff_query_complexity()
    FLPCPCoeffSqrt.flpcp_coeff_prover_time()
    FLPCPCoeffSqrt.flpcp_coeff_verifier_time()

    # FLPCP Sqrt. vs FLPCPCoeff Sqrt.
    FLPCPCoeffSqrt.compare_prover_time()
    FLPCPCoeffSqrt.compare_verifier_time()

    # FLIOP
    FLIOP.proof_size()
    FLIOP.query_complexity()
    FLIOP.prover_time()
    FLIOP.verifier_time()

    # FLIOPCoeff
    FLIOPCoeff.proof_size()
    FLIOPCoeff.query_complexity()
    FLIOPCoeff.prover_time()
    FLIOPCoeff.verifier_time()

    # FLIOP vs FLIOPCoeff
    FLIOPCoeff.compare_prover_time()
    FLIOPCoeff.compare_verifier_time()

    # Network Simulation
    lan()
    wan()
