export class GeoLocation {
    countryCode: string;
    continent: string;
    country: string;

    constructor(options: {
        countryCode: string;
        continent: string;
        country;

    }) {
        this.countryCode = options.countryCode;
        this.continent = options.continent;
        this.country = options.country;
    }
}